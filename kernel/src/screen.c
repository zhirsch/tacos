#include "screen.h"

#include <stdarg.h>
#include <stdint.h>

#include "portio.h"
#include "snprintf.h"
#include "string.h"

#define VGAMEMBASE ((uint16_t*)0x000B8000)
#define MAXLINES 25
#define MAXCOLS 80

static int attrib = 0x07;
static struct { char x, y; } cursor = { 0, 0 };

static void scroll(void);
static void update_cursor(void);

void screen_clear(void) {
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

void screen_panic(void) {
  for (int i = 0; i < MAXLINES; i++) {
    for (int j = 0; j < MAXCOLS; j++) {
      unsigned short* p = VGAMEMBASE + i * MAXCOLS + j;
      *p = (0x4f << 8) | (*p & 0xff);
    }
  }
}

void screen_writech(char ch) {
  if (ch >= ' ') {
    *(VGAMEMBASE + (cursor.y * MAXCOLS + cursor.x)) = ch | (attrib << 8);
    cursor.x++;
  }
  if (ch == '\n' || cursor.x >= MAXCOLS) {
    // Move to the next line.
    cursor.x = 0;
    cursor.y++;
  }
  scroll();
  update_cursor();
}

void screen_writef(const char* fmt, ...) {
  va_list ap;
  char s[256];
  int c;

  va_start(ap, fmt);
  c = vsnprintf(s, sizeof(s) - 1, fmt, ap);
  va_end(ap);

  for (int i = 0; i < c; i++) {
    screen_writech(s[i]);
  }
}

static void scroll(void) {
  if (cursor.y < MAXLINES) {
    return;
  }

  // Move lines 1 through MAXLINES up one.
  memcpy(VGAMEMBASE, VGAMEMBASE + MAXCOLS, (MAXLINES - 1) * MAXCOLS * 2);

  // Blank out the last line.
  for (int i = 0; i < MAXCOLS; i++) {
    unsigned short* p = VGAMEMBASE + (MAXLINES - 1) * MAXCOLS + i;
    *p = ' ' | (attrib << 8);
  }

  // Set the cursor to the last line.
  cursor.y = MAXLINES - 1;
}

static void update_cursor(void) {
  const uint16_t position = (cursor.y * MAXCOLS) + cursor.x;

  // cursor LOW port to vga INDEX register
  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)((position & 0x00FF) >> 0));
  // cursor HIGH port to vga INDEX register
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((position & 0xFF00) >> 8));
}
