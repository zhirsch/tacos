#include "ide/ide.h"

#include <stdint.h>

#include "interrupts.h"
#include "kmalloc.h"
#include "kprintf.h"
#include "portio.h"

#define NUM_CONTROLLERS 2
#define NUM_DEVICES_PER_CONTROLLER 2

#define ATA_REGISTER_DATA             0x000
#define ATA_REGISTER_FEATURES         0x001
#define ATA_REGISTER_INTERRUPT_REASON 0x002
#define ATA_REGISTER_LBA_LOW          0x003
#define ATA_REGISTER_BYTE_LOW         0x004
#define ATA_REGISTER_BYTE_HIGH        0x005
#define ATA_REGISTER_DEVICE           0x006
#define ATA_REGISTER_COMMAND          0x007
#define ATA_REGISTER_STATUS           0x007
#define ATA_REGISTER_ALTSTATUS        0x206

#define ATA_STATUS_ERR  (1 << 0)
#define ATA_STATUS_DRQ  (1 << 3)
#define ATA_STATUS_DRDY (1 << 6)
#define ATA_STATUS_BSY  (1 << 7)

#define ATA_COMMAND_IDENTIFY_DEVICE        0xEC
#define ATA_COMMAND_PACKET                 0xA0
#define ATA_COMMAND_IDENTIFY_PACKET_DEVICE 0xA1

// A device attached to an IDE controller.
struct ide_device {
  // The controller, and position on that controller, of this device.
  struct ide_controller* controller;
  int position;

  // Whether this device is actually present.  The rest of the fields below here
  // are only valid if this field is true.
  int present;

  // Whether this is an ATAPI device.
  int atapi;

  // Human-readable strings identifying this device.
  char model[41];
  char serial[21];
  char firmware[9];
};

// An IDE controller.
struct ide_controller {
  // The base I/O port for communication with devices on this controller.
  int iobase;

  // The signal that an IRQ has been received for this controller.
  volatile int irq_signal;

  // The devices attached to this controller.
  struct ide_device devices[NUM_DEVICES_PER_CONTROLLER];
};

// The controllers and devices.  Only 2 controller and 2 devices are supported.
static struct ide_controller controllers[NUM_CONTROLLERS] = {
  {
    .iobase = 0x1f0,
    .irq_signal = 0,
    .devices = {
      { .position = 0, },
      { .position = 1, },
    },
  },
  {
    .iobase = 0x170,
    .irq_signal = 0,
    .devices = {
      { .position = 0, },
      { .position = 1, },
    },
  },
};

static void wait_for_400ns(struct ide_controller* controller) {
  // Each time a port is read, it takes 100 ns.  Reading the ALTSTATUS register
  // doesn't affect the state of the controller, so read it 4 times.
  inb(controller->iobase + ATA_REGISTER_ALTSTATUS);
  inb(controller->iobase + ATA_REGISTER_ALTSTATUS);
  inb(controller->iobase + ATA_REGISTER_ALTSTATUS);
  inb(controller->iobase + ATA_REGISTER_ALTSTATUS);
}

static int select_device(struct ide_device* device) {
  const int iobase = device->controller->iobase;

  // The controller must not be in the BSY or DRQ states.
  if (inb(iobase + ATA_REGISTER_STATUS) & (ATA_STATUS_DRQ | ATA_STATUS_BSY)) {
    return 0;
  }

  // Select the drive.
  outb(iobase + ATA_REGISTER_DEVICE, 0xA0 | (device->position << 4));
  wait_for_400ns(device->controller);

  // Again, the controller should not be BSY or DRQ.
  if (inb(iobase + ATA_REGISTER_STATUS) & (ATA_STATUS_DRQ | ATA_STATUS_BSY)) {
    return 0;
  }

  return 1;
}

static void swap(char* a, char* b) {
  const char x = *a;
  *a = *b;
  *b = x;
}

static void fix_string(char* str, int n) {
  // Ensure that there's a NUL byte at the end.
  str[n - 1] = '\0';
  // Swap every pair of two charaters.
  for (int i = 0; i < n - 1; i += 2) {
    swap(str + i, str + i + 1);
  }
  // Remove space padding at the end of the string.
  for (int i = n - 2; i >= 0 && str[i] == ' '; i--) {
    str[i] = '\0';
  }
}

static void identify_ide_device(struct ide_device* device) {
  const int iobase = device->controller->iobase;
  uint16_t* buffer;
  int status;

  device->present = 0;

  // Select the device.
  if (!select_device(device)) {
    return;
  }

  // Try to identify the device as ATA.
  outb(iobase + ATA_REGISTER_COMMAND, ATA_COMMAND_IDENTIFY_DEVICE);
  wait_for_400ns(device->controller);

  // If the status is 0x00 or 0xff, then there's no device.
  status = inb(iobase + ATA_REGISTER_STATUS);
  if (status == 0x00 || status == 0xff) {
    return;
  }

  // Wait for BSY to clear and DRQ to be set.
  while (1) {
    if (!(status & ATA_STATUS_BSY) && (status & ATA_STATUS_DRQ)) {
      break;
    }
    // If an error occured and the command registers are set how to spec defines
    // for an ATAPI device, send an IDENTIFY PACKET DEVICE command.
    if (status & ATA_STATUS_ERR) {
      if (!device->atapi &&
          inb(iobase + ATA_REGISTER_INTERRUPT_REASON) == 0x01 &&
          inb(iobase + ATA_REGISTER_LBA_LOW)          == 0x01 &&
          inb(iobase + ATA_REGISTER_BYTE_LOW)         == 0x14 &&
          inb(iobase + ATA_REGISTER_BYTE_HIGH)        == 0xEB &&
          inb(iobase + ATA_REGISTER_DEVICE) & ATA_COMMAND_IDENTIFY_DEVICE) {
        device->atapi = 1;
        outb(iobase + ATA_REGISTER_COMMAND, ATA_COMMAND_IDENTIFY_PACKET_DEVICE);
        wait_for_400ns(device->controller);
      } else {
        return;
      }
    }
    status = inb(iobase + ATA_REGISTER_STATUS);
  }

  device->present = 1;

  // Read the data.
  buffer = kmalloc(256 * sizeof(*buffer));
  for (int i = 0; i < 256; i++) {
    buffer[i] = inw(iobase + ATA_REGISTER_DATA);
  }

  // Set the device metadata.
  __builtin_memcpy(device->model, buffer + 27, sizeof(device->model) - 1);
  fix_string(device->model, sizeof(device->model));
  __builtin_memcpy(device->serial, buffer + 10, sizeof(device->serial) - 1);
  fix_string(device->serial, sizeof(device->serial));
  __builtin_memcpy(device->firmware, buffer + 23, sizeof(device->firmware) - 1);
  fix_string(device->firmware, sizeof(device->firmware));

  kfree(buffer);
}

static void wait_for_irq(struct ide_controller* controller) {
  while (!controller->irq_signal) {
    __asm__ __volatile__ ("pause");
  }
  controller->irq_signal = 0;
}

static void primary_controller_handler(struct isr_frame* frame) {
  controllers[0].irq_signal = 1;
}

static void secondary_controller_handler(struct isr_frame* frame) {
  controllers[1].irq_signal = 1;
}

void init_ide(void) {
  interrupt_register_handler(0x2e, primary_controller_handler);
  interrupt_register_handler(0x2f, secondary_controller_handler);

  for (int i = 0; i < NUM_CONTROLLERS; i++) {
    for (int j = 0; j < NUM_DEVICES_PER_CONTROLLER; j++) {
      // Identify this device.
      struct ide_device* device = &controllers[i].devices[j];
      device->controller = &controllers[i];
      identify_ide_device(device);

      // If the device isn't present, or it's not ATAPI, skip it.
      // TODO(zhirsch): Support ATAPI.
      if (!device->present || !device->atapi) {
        continue;
      }

      kprintf("IDE: ATAPI CDROM detected at %d-%d\n", i, j);
      kprintf("IDE:     serial:   %s\n", device->serial);
      kprintf("IDE:     model:    %s\n", device->model);
      kprintf("IDE:     firmware: %s\n", device->firmware);
    }
  }
}

static uint8_t wait_while(struct ide_controller* controller, int while_status) {
  const int iobase = controller->iobase;
  uint8_t status = inb(iobase + ATA_REGISTER_STATUS);
  while (status & while_status) {
    __asm__ __volatile__ ("pause");
    status = inb(iobase + ATA_REGISTER_STATUS);
  }
  return status;
}

static uint8_t wait_until(struct ide_controller* controller, int until_status) {
  const int iobase = controller->iobase;
  uint8_t status = inb(iobase + ATA_REGISTER_STATUS);
  while (!(status & until_status)) {
    __asm__ __volatile__ ("pause");
    status = inb(iobase + ATA_REGISTER_STATUS);
  }
  return status;
}

// Waits while the controller is BSY, then returns true if the device is DRQ.
static int wait_for_bsy_to_drq(struct ide_controller* controller) {
  const uint8_t status = wait_while(controller, ATA_STATUS_BSY);

  // Return false if ERR.
  if (status & ATA_STATUS_ERR) {
    kprintf("IDE: error status: %02x", status);
    return 0;
  }

  // Return false if not DRQ.
  if (!(status & ATA_STATUS_DRQ)) {
    kprintf("IDE: unexpected status: %02x", status);
    return 0;
  }

  return 1;
}

int ide_read(int controller, int position, void* buffer, int lba, int nwords) {
  struct ide_device* device;
  int iobase;
  uint8_t packet[12];

  // Make sure that the device to read from is valid.
  if (controller >= NUM_CONTROLLERS || position >= NUM_DEVICES_PER_CONTROLLER) {
    return 0;
  }
  device = &controllers[controller].devices[position];
  if (!device->present || !device->atapi) {
    return 0;
  }

  iobase = device->controller->iobase;

  // Select the device.
  if (!select_device(device)) {
    return 0;
  }

  // Enable PIO mode.
  outb(iobase + ATA_REGISTER_FEATURES, 0x0);

  // Set the number of bytes to read.
  outb(iobase + ATA_REGISTER_BYTE_LOW,  ((nwords * 2) >> 0) & 0xFF);
  outb(iobase + ATA_REGISTER_BYTE_HIGH, ((nwords * 2) >> 8) & 0xFF);

  // Send the PACKET command to read bytes.
  // TODO(zhirsch): Align to sector boundaries? At least set the number of
  // sectors to read appropriately based on nwords.
  packet[0x0] = 0xA8;
  packet[0x1] = 0x0;
  packet[0x2] = (lba >> 24) & 0xFF;
  packet[0x3] = (lba >> 16) & 0xFF;
  packet[0x4] = (lba >>  8) & 0xFF;
  packet[0x5] = (lba >>  0) & 0xFF;
  packet[0x6] = 0x0;
  packet[0x7] = 0x0;
  packet[0x8] = 0x0;
  packet[0x9] = nwords / 2048;
  if ((nwords % 2048) > 0) {
    packet[0x9]++;
  }
  packet[0xa] = 0x0;
  packet[0xb] = 0x0;
  outb(iobase + ATA_REGISTER_COMMAND, ATA_COMMAND_PACKET);
  wait_for_400ns(device->controller);

  // Wait for DRQ.
  if (!wait_for_bsy_to_drq(device->controller)) {
    return 0;
  }

  // Write the command packet.
  for (unsigned int i = 0; i < sizeof(packet) / 2; i++) {
    outw(iobase + ATA_REGISTER_DATA, ((uint16_t*)packet)[i]);
  }

  // Wait for DRQ.
  wait_for_irq(device->controller);
  if (!wait_for_bsy_to_drq(device->controller)) {
    return 0;
  }

  // Read the data.
  for (int i = 0; i < nwords; i++) {
    ((uint16_t*)buffer)[i] = inw(iobase + ATA_REGISTER_DATA);
  }

  // Wait for DRDY.  This ensures that the device is back in a known state.
  wait_for_irq(device->controller);
  wait_until(device->controller, ATA_STATUS_DRDY);

  return 1;
}
