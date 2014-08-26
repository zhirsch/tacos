#include "iso9660.h"

#include <stddef.h>
#include <stdint.h>

#include "ide/ide.h"
#include "kmalloc.h"
#include "kprintf.h"

void* iso9660_load_file_from_atapi(int controller, int position, const char* path) {
  uint32_t extent_pos, extent_len;

  if (path[0] != '/') {
    kprintf("ISO9660: Load path %s does not start with a slash\n", path);
    return NULL;
  }

  // Find the primary volume descriptor.
  while (1) {
    uint8_t buffer[2048];
    int lba = 0x10;
    if (!ide_read(controller, position, buffer, lba, sizeof(buffer) / 2)) {
      kprintf("ISO9660: Read from %d-%d at LBA %x of %lx bytes failed\n", controller, position, lba, sizeof(buffer));
      return NULL;
    }
    if (buffer[0] == 0x1) {
      path += __builtin_strlen("/");
      __builtin_memcpy(&extent_pos, buffer + 156 +  2, 4);
      __builtin_memcpy(&extent_len, buffer + 156 + 10, 4);
      break;
    } else if (buffer[0] == 0xff) {
      kprintf("ISO9660: No primary volume descriptor found on %d-%d\n", controller, position);
      return NULL;
    }
  }

  while (1) {
    int i = 0;
    uint8_t buffer[2048];

    if (!ide_read(controller, position, buffer, extent_pos, extent_len / 2)) {
      kprintf("ISO9660: Read from %d-%d at LBA %lx of %lx bytes failed\n", controller, position, extent_pos, extent_len);
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
      if (__builtin_strlen(path) < namelen ||
          __builtin_strstr(name, path) != name) {
        continue;
      }
      if (path[namelen] == '\0' && !(p[25] & 0x2)) {
        void* ptr;
        __builtin_memcpy(&extent_pos, p +  2, 4);
        __builtin_memcpy(&extent_len, p + 10, 4);
        ptr = kmemalign(0x1000, extent_len);
        if (ptr == NULL) {
          panic("ISO9660: Unable to allocate memory to load file.\n");
        }
        if (!ide_read(controller, position, ptr, extent_pos, extent_len / 2)) {
          kprintf("ISO9960: Read from %d-%d at LBA %lx of %lx bytes failed\n", controller, position, extent_pos, extent_len);
          kfree(ptr);
          return NULL;
        }
        return ptr;
      }
      if (path[namelen] == '/') {
        path += namelen + 1;
        __builtin_memcpy(&extent_pos, p +  2, 4);
        __builtin_memcpy(&extent_len, p + 10, 4);
        break;
      }
    }
    if (buffer[i] == 0) {
      kprintf("SAD FACE!\n");
      return NULL;
    }
  }
}
