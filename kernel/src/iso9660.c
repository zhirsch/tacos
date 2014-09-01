#include "iso9660.h"

#include <stddef.h>
#include <stdint.h>

#include "ide/ide.h"
#include "kmalloc.h"
#include "log.h"
#include "mmu.h"
#include "string.h"

#define LOG(...) log("ISO9660", __VA_ARGS__)
#define PANIC(...) panic("ISO9660", __VA_ARGS__)

#define BUFFER_SIZE 2048

void* iso9660_load_file_from_atapi(int controller, int position, const char* path) {
  uint32_t extent_pos, extent_len;
  uint8_t* buffer;

  if (path[0] != '/') {
    LOG("Load path %s does not start with a slash\n", path);
    return NULL;
  }

  // Find the primary volume descriptor.
  buffer = kmalloc(BUFFER_SIZE);
  while (1) {
    int lba = 0x10;
    if (!ide_read(controller, position, buffer, lba, BUFFER_SIZE / 2)) {
      LOG("Read from %d-%d at LBA %x of %x bytes failed\n", controller, position, lba, BUFFER_SIZE);
      kfree(buffer);
      return NULL;
    }
    if (buffer[0] == 0x1) {
      path += strlen("/");
      memcpy(&extent_pos, buffer + 156 +  2, 4);
      memcpy(&extent_len, buffer + 156 + 10, 4);
      break;
    } else if (buffer[0] == 0xff) {
      LOG("No primary volume descriptor found on %d-%d\n", controller, position);
      kfree(buffer);
      return NULL;
    }
  }

  while (1) {
    int i = 0;

    if (!ide_read(controller, position, buffer, extent_pos, extent_len / 2)) {
      LOG("Read from %d-%d at LBA %lx of %lx bytes failed\n", controller, position, extent_pos, extent_len);
      kfree(buffer);
      return NULL;
    }

    // Skip the first two records (. and ..).
    i += buffer[i];
    i += buffer[i];

    // Look for the entry that matches the path.
    while (buffer[i] != 0) {
      const uint8_t* p = buffer + i;
      const char* name = (const char*)(p + 33);
      const uint8_t namelen = p[32];
      i += p[0];
      if (strlen(path) < namelen) {
        continue;
      }
      {
        int i;
        for (i = 0; i < namelen; i++) {
          if (path[i] == '/' || path[i] == '\0') {
            break;
          }
          if (path[i] != name[i]) {
            break;
          }
        }
        if (i != namelen) {
          continue;
        }
      }
      if (path[namelen] == '\0' && !(p[25] & 0x2)) {
        void* ptr;
        memcpy(&extent_pos, p +  2, 4);
        memcpy(&extent_len, p + 10, 4);
        ptr = kmemalign(PAGESIZE, extent_len);
        if (ptr == NULL) {
          PANIC("Unable to allocate memory to load file.\n");
        }
        LOG("Need to read %08lx words from %d-%d at LBA %lx\n", extent_len / 2, controller, position, extent_pos);
        if (!ide_read(controller, position, ptr, extent_pos, extent_len / 2)) {
          LOG("Read from %d-%d at LBA %lx of %lx bytes failed\n", controller, position, extent_pos, extent_len);
          kfree(ptr);
          kfree(buffer);
          return NULL;
        }
        kfree(buffer);
        return ptr;
      }
      if (path[namelen] == '/') {
        path += namelen + 1;
        memcpy(&extent_pos, p +  2, 4);
        memcpy(&extent_len, p + 10, 4);
        break;
      }
    }
    if (buffer[i] == 0) {
      LOG("SAD FACE!\n");
      kfree(buffer);
      return NULL;
    }
  }
}
