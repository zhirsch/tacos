#include "iso9660.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bits/errno.h"
#include "bits/fcntl.h"
#include "bits/types.h"

#include "ide/ide.h"
#include "kmalloc.h"
#include "log.h"
#include "mmu.h"
#include "string.h"

#define LOG(...) log("ISO9660", __VA_ARGS__)
#define PANIC(...) panic("ISO9660", __VA_ARGS__)

#define SECTOR_SIZE 2048

#define PRIMARY_VOLUME_DESCRIPTOR  0x01
#define VOLUME_DESCRIPTOR_SENTINEL 0xff

struct directory_entry {
  uint8_t len;
  uint8_t xattr_len;

  uint32_t lba_lsb;
  uint32_t lba_msb;

  uint32_t size_lsb;
  uint32_t size_msb;

  char timestamp[7];

  uint8_t flags;

  uint8_t unit_size;
  uint8_t gap_size;

  uint16_t volume_lsb;
  uint16_t volume_msb;

  uint8_t path_len;
} __attribute__ ((packed));

struct date_time {
  char year[4];
  char month[2];
  char day[2];
  char hour[2];
  char minute[2];
  char second[2];
  char hundreths[2];
  uint8_t tzoffset;
} __attribute__ ((packed));

struct primary_volume_descriptor {
  uint8_t type;
  char    id[5];
  uint8_t version;

  uint8_t unused1;

  char system_id[32];
  char volume_id[32];

  char unused2[8];

  uint32_t volume_size_lsb;
  uint32_t volume_size_msb;

  char unused3[32];

  uint16_t volume_set_size_lsb;
  uint16_t volume_set_size_msb;

  uint16_t volume_sequence_numer_lsb;
  uint16_t volume_sequence_numer_msb;

  uint16_t logical_block_size_lsb;
  uint16_t logical_block_size_msb;

  uint32_t path_table_size_lsb;
  uint32_t path_table_size_msb;

  uint32_t path_table_lba_lsb;
  uint32_t optional_path_table_lba_lsb;

  uint32_t path_table_lba_msb;
  uint32_t optional_path_table_lba_msb;

  struct directory_entry root;
  char padding;

  char volume_set_id[128];
  char publisher_id[128];
  char data_preparer_id[128];
  char application_id[128];
  char copyright_file_name[38];
  char abstract_file_name[36];
  char bibliographic_file_name[37];

  struct date_time volume_creation_date_time;
  struct date_time volume_modification_date_time;
  struct date_time volume_expiration_date_time;
  struct date_time volume_effection_date_time;

  uint8_t file_structure_version;

  uint8_t unused4;

  char application[512];
  char reserved[653];
} __attribute__ ((packed));

static struct primary_volume_descriptor* get_primary_volume_descriptor(void);
static off_t get_directory_entry_offset(char* path, struct directory_entry* entry);
static struct directory_entry* get_directory_entry(off_t entry_offset);

int iso9660_open(const char* path, int flags, struct file** file) {
  struct primary_volume_descriptor* pvd = NULL;
  char* p = NULL;

  if (path[0] != '/') {
    LOG("Path does not start with a slash: %s\n", path);
    return -ENOENT;
  }
  if (flags != O_RDONLY) {
    LOG("ISO9660 files can only be opened read-only: %08x\n", flags);
    return -EACCES;
  }

  // Find the primary volume descriptor.
  pvd = get_primary_volume_descriptor();
  if (pvd == NULL) {
    return -EIO;
  }

  // Find the path's directory entry.
  p = strdup(path);
  *file = kmalloc(sizeof(**file));
  (*file)->ref = 1;
  (*file)->inode = get_directory_entry_offset(p + 1, &pvd->root);
  kfree(p);
  if ((*file)->inode == 0) {
    LOG("Unable to find directory entry for %s\n", path);
    kfree(pvd);
    kfree(*file);
    *file = NULL;
    return -ENOENT;
  }

  (*file)->path = strdup(path);
  kfree(pvd);
  return 0;
}

int iso9660_fstat(struct file* file, struct stat* st) {
  struct directory_entry* entry = NULL;

  entry = get_directory_entry(file->inode);
  if (entry == NULL) {
    return -EIO;
  }

  st->st_dev     = 0;  // TODO
  st->st_ino     = file->inode;
  st->st_mode    = (entry->flags & 0x1) ? S_IFDIR : S_IFREG;
  st->st_nlink   = 1;  // TODO
  st->st_uid     = 0;  // TODO
  st->st_gid     = 0;  // TODO
  st->st_rdev    = 0;  // TODO
  st->st_size    = entry->size_lsb;
  st->st_blksize = SECTOR_SIZE;
  st->st_blocks  = entry->size_lsb / 512 + (entry->size_lsb % 512 > 0);
  st->st_atime   = 0;  // unknown
  st->st_mtime   = 0;  // TODO
  st->st_ctime   = 0;  // unknown
  kfree(entry);
  return 0;
}

int iso9660_pread(struct file* file, void* buf, size_t count, off_t offset) {
  struct directory_entry* entry = NULL;
  int lba;
  int sectors;
  uint8_t* temp = NULL;

  // Get the metadata.
  entry = get_directory_entry(file->inode);
  if (entry == NULL) {
    return -EIO;
  }

  // Read the sectors.
  lba = (entry->lba_lsb * SECTOR_SIZE + offset) / SECTOR_SIZE;
  sectors = count / SECTOR_SIZE + (count % SECTOR_SIZE > 0);
  temp = kmalloc(sectors * SECTOR_SIZE);
  if (!ide_read(0, 0, temp, lba, sectors * SECTOR_SIZE)) {
    kfree(temp);
    return -EIO;
  }

  // Copy the appropriate bytes into the buffer.
  memcpy(buf, temp + offset % SECTOR_SIZE, count);
  kfree(temp);
  return count;
}

int iso9660_close(struct file* file) {
  --(file->ref);
  if (file->ref == 0) {
    kfree((char*)file->path);
    kfree(file);
  }
  return 0;
}

static struct primary_volume_descriptor* get_primary_volume_descriptor(void) {
  int lba = 0x10;
  struct primary_volume_descriptor* pvd = NULL;

  // Allocate space for the primarh volume descriptor.
  pvd = kmalloc(sizeof(*pvd));
  if (pvd == NULL) {
    LOG("Unable to allocate space for the primary volume descriptor\n");
    return NULL;
  }

  // Read volume descriptors until the primary one is found, or all the
  // descriptors have been read.
  do {
    if (!ide_read(0, 0, pvd, lba, SECTOR_SIZE)) {
      LOG("Unable to read volume descriptor\n");
      kfree(pvd);
      return NULL;
    }
    if (pvd->type == PRIMARY_VOLUME_DESCRIPTOR) {
      return pvd;
    }
  } while (pvd->type != VOLUME_DESCRIPTOR_SENTINEL);

  LOG("Unable to find primary volume descriptor\n");
  kfree(pvd);
  return NULL;
}

static char* decode_directory_entry_path(struct directory_entry* d) {
  const char* dpath = (const char*)d + sizeof(*d);
  char* p = NULL;
  const int align = ((d->path_len % 2) == 0);
  int i;

  // Handle special cases for the . and .. entries.
  if (d->path_len == 1 && *dpath == 0x00) {
    return strdup(".");
  } else if (d->path_len == 1 && *dpath == 0x01) {
    return strdup("..");
  }

  // Parse the rock ridge extensions.
  // TODO: Read the SP entry in the . entry of the root directory table to
  // determine the offset from the start of the system use area.  For now,
  // assume that it's zero.
#define RR_SU_OFFSET 0
  i = sizeof(*d) + d->path_len + align + RR_SU_OFFSET;
  while (i < d->len) {
    const uint8_t* su = (uint8_t*)d + i;
    const uint16_t sig = (su[0] << 8) | (su[1] << 0);
    const uint8_t  len = su[2];
    if (sig != 0x4e4d) { // NM
      i += len;
      continue;
    }
    p = kmalloc(len - 5 + 1);
    memcpy(p, su + 5, len - 5);
    p[len - 5] = '\0';
    return p;
  }

  p = kmalloc(d->path_len + 1);
  memcpy(p, dpath, d->path_len);
  p[d->path_len] = '\0';
  return p;
}

static off_t get_directory_entry_offset(char* path, struct directory_entry* entry) {
  const char* p = NULL;
  uint8_t* buffer = NULL;
  uint32_t i;

  // Get the first path component.
  p = strsep(&path, "/");

  // Allocate space for the directory entries.
  buffer = kmalloc(entry->size_lsb);
  if (buffer == NULL) {
    LOG("Unable to allocate space for directory entries\n");
    return 0;
  }

  // Read the directory entries.
  if (!ide_read(0, 0, buffer, entry->lba_lsb, entry->size_lsb)) {
    LOG("Unable to read the directory entries\n");
    kfree(buffer);
    return 0;
  }

  // Find the directory entry that matches the path component.
  i = 0;
  while (i < entry->size_lsb) {
    off_t pos;
    struct directory_entry* d = (struct directory_entry*)(buffer + i);
    char* dpath = NULL;
    if (d->len == 0) {
      break;
    }
    dpath = decode_directory_entry_path(d);
    if (strcmp(p, dpath) != 0) {
      i += d->len;
      kfree(dpath);
      continue;
    }
    if (path == NULL) {
      kfree(dpath);
      kfree(buffer);
      return entry->lba_lsb * SECTOR_SIZE + i;
    }
    pos = get_directory_entry_offset(path, d);
    kfree(dpath);
    kfree(buffer);
    return pos;
  }

  kfree(buffer);
  return 0;
}

static struct directory_entry* get_directory_entry(off_t entry_offset) {
  int lba;
  uint8_t* buffer = NULL;
  size_t len;
  struct directory_entry* entry = NULL;

  buffer = kmalloc(SECTOR_SIZE);
  if (buffer == NULL) {
    LOG("Unable to allocate space for the directory entry sector\n");
    return NULL;
  }

  lba = entry_offset / SECTOR_SIZE;
  if (!ide_read(0, 0, buffer, lba, SECTOR_SIZE)) {
    LOG("Unable to read the directory entry sector\n");
    return NULL;
  }

  entry = (struct directory_entry*)(buffer + entry_offset % SECTOR_SIZE);
  len = sizeof(*entry) + entry->path_len;
  entry = kmalloc(len);
  if (entry == NULL) {
    LOG("Unable to allocate space for the directory entry\n");
    return NULL;
  }

  memcpy(entry, buffer + entry_offset % SECTOR_SIZE, len);
  kfree(buffer);
  return entry;
}
