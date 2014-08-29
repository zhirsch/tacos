#include "screen.h"

#include "portio.h"

#define VGAMEMBASE ((unsigned short*)0x000B8000)
#define MAXLINES 25
#define MAXCOLS 80

static int attrib = 0x07;
static struct { char x, y; } cursor = { 0, 0 };

static void scroll(void);

void clearscreen(void) {
  // Write a space to every position.
  for (int i = 0; i < MAXLINES; i++) {
    for (int j = 0; j < MAXCOLS; j++) {
      unsigned short* p = VGAMEMBASE + i * MAXCOLS + j;
      *p = ' ' | (attrib << 8);
    }
  }

  // Reset the cursor to the top left.
  cursor.x = 0;
  cursor.y = 0;
}

void puts(const char* text) {
  // Write each char.
  for (const char* p = text; *p != '\0'; p++) {
    putch(*p);
  }

  // Update the cursor position.
  {
    const unsigned short position = (cursor.y * 80) + cursor.x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)((position >> 0) & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
  }
}

void putch(char ch) {
  if (ch >= ' ') {
    // Write the character to the screen.
    unsigned short* p = VGAMEMBASE + (cursor.y * MAXCOLS + cursor.x);
    *p = ch | (attrib << 8);
    outb(0xe9, ch);
    cursor.x++;
  }
  if (ch == '\n' || cursor.x >= MAXCOLS) {
    // Move to the next line.
    cursor.x = 0;
    cursor.y++;
    if (ch == '\n') {
      outb(0xe9, '\n');
    }
  }
  scroll();
}

static void scroll(void) {
  if (cursor.y < MAXLINES) {
    return;
  }

  // Move lines 1 through MAXLINES up one.
  __builtin_memcpy(VGAMEMBASE, VGAMEMBASE + MAXCOLS, (MAXLINES - 1) * MAXCOLS * 2);

  // Blank out the last line.
  for (int i = 0; i < MAXCOLS; i++) {
    unsigned short* p = VGAMEMBASE + (MAXLINES - 1) * MAXCOLS + i;
    *p = ' ' | (attrib << 8);
  }

  // Set the cursor to the last line.
  cursor.y = MAXLINES - 1;
}
