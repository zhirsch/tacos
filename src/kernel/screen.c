/*****************************************************************************
 * screen.c
 *****************************************************************************/

#include <tacos/screen.h>
#include <lib/string.h>

/* The base pointer for the video memory */
#define VGAMEMBASE ((unsigned short *)0xC00B8000)

/* The max number of lines on the screen */
#define MAXLINES 25

/* The max number of columns on the screen */
#define MAXCOLS 80

/* The current text attributes (color, etc.) */
static int attrib = 0x07;

/* The current cursor position */
static struct { int x, y; } cursor = { 0, 0 };

/*****************************************************************************
 * scroll --
 *   Scrolls the screen up by one line if needed.
 *****************************************************************************/
static void scroll(void)
{
   if (cursor.y >= MAXLINES)
   {
      /* A blank is a space -- use the current attributes */
      unsigned short blank = ' ' | (attrib << 8);

      /* Move line 1 through MAXLINES up one line */
      memcpy(VGAMEMBASE, VGAMEMBASE + MAXCOLS, (MAXLINES - 1) * MAXCOLS * 2);

      /* Blank out the last line */
      memsetw(VGAMEMBASE + (MAXLINES - 1) * MAXCOLS, blank, MAXCOLS);

      /* Set the cursor to be the last line */
      cursor.y = MAXLINES - 1;
   }
}

/*****************************************************************************
 * show_hardware_cursor
 *   Shows the hardware cursor on the screen at the software cursor position.
 *****************************************************************************/
static void show_hardware_cursor(void)
{
#if 0
   unsigned int pos = cursor.y * 80 + cursor.x;

   /* XXX: not sure how this works yet. */
   outb(0x3D4, 14);
   outb(0x3D5, pos >> 8);
   outb(0x3D4, 15);
   outb(0x3D5, pos);
#endif
}

/**
 * clearscreen
 *
 * Clear the screen.
 */
void clearscreen(void)
{
   unsigned int blank;
   int i;

   blank = ' ' | (attrib << 8);

   for (i = 0; i < MAXLINES; i++)
      memsetw(VGAMEMBASE + i * MAXCOLS, blank, MAXCOLS);

   cursor.x = 0;
   cursor.y = 0;
   show_hardware_cursor();
}

/**
 * putch
 *
 * Puts a character onto the screen at the current cursor position.
 */
void putch(unsigned char ch)
{
   unsigned short *where;

   if (ch == 0x08) { /* backspace */
      if (cursor.x > 0)
         cursor.x--;
   }
   else if (ch == '\t') {
      /* make sure the last 3 bits are 0 -- 8 space aligned tabs */
      cursor.x = (cursor.x + 8) & ~(8 - 1);
   }
   else if (ch == '\r') {
      /* carriage return -- move to beginning of the line */
      cursor.x = 0;
   }
   else if (ch == '\n') {
      /* treat a new line as \r\n -- move to the beginning of the next line */
      cursor.x = 0;
      cursor.y++;
   }
   else if (ch >= ' ') {
      /* anything >= a space is printable, so print it at the current position
         and move to the next column */
      where = VGAMEMBASE + (cursor.y * MAXCOLS + cursor.x);
      *where = ch | (attrib << 8);
      cursor.x++;
   }

   /* if the cursor is past the last column, move to the beginning of the
      next line */
   if (cursor.x >= MAXCOLS) {
      cursor.x = 0;
      cursor.y++;
   }

   /* scroll (if necessary) and show the hardware cursor in the new position */
   scroll();
   show_hardware_cursor();
}

void puts(const char *text)
{
   while (*text)
      putch(*text++);
}
