#ifndef IDE_IDE_H
#define IDE_IDE_H

#include <stdint.h>

// Initialize IDE devices.  Must be called before any other IDE functions can be
// called.
void init_ide(void);

// Read bytes from an IDE device.  The device is identified by its position on a
// controller.  The lba is the sector offset, and nwords is the number of words
// to read into the buffer.
int ide_read(int controller, int position, uint16_t* buffer, int lba, int nwords);

#endif /* IDE_IDE_H */
