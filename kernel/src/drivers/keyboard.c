#include "drivers/keyboard.h"

#include <stdbool.h>

#include "interrupts.h"
#include "log.h"
#include "portio.h"
#include "process.h"
#include "tty.h"

#define LOG(...) log("DRIVER [keyboard]", __VA_ARGS__)

static bool g_left_shift_down = false;

/*
0xE0, 0x1C	(keypad) enter pressed
0xE0, 0x1D	right control pressed				
0xE0, 0x35	(keypad) / pressed				
0xE0, 0x38	right alt (or altGr) pressed						
0xE0, 0x47	home pressed
0xE0, 0x48	cursor up pressed
0xE0, 0x49	page up pressed
0xE0, 0x4B	cursor left pressed
0xE0, 0x4D	cursor right pressed
0xE0, 0x4F	end pressed
0xE0, 0x50	cursor down pressed
0xE0, 0x51	page down pressed
0xE0, 0x52	insert pressed
0xE0, 0x53	delete pressed
0xE0, 0x5B	left GUI pressed
0xE0, 0x5C	right GUI pressed
0xE0, 0x5D	"apps" pressed				
0xE0, 0x9C	(keypad) enter released
0xE0, 0x9D	right control released				
0xE0, 0xB5	(keypad) / released				
0xE0, 0xB8	right alt (or altGr) released						
0xE0, 0xC7	home released
0xE0, 0xC8	cursor up released
0xE0, 0xC9	page up released
0xE0, 0xCB	cursor left released
0xE0, 0xCD	cursor right released
0xE0, 0xCF	end released
0xE0, 0xD0	cursor down released
0xE0, 0xD1	page down released
0xE0, 0xD2	insert released
0xE0, 0xD3	delete released
0xE0, 0xDB	left GUI released
0xE0, 0xDC	right GUI released
0xE0, 0xDD	"apps" released				
0xE0, 0x2A, 0xE0, 0x37	print screen pressed
0xE0, 0xB7, 0xE0, 0xAA	print screen released
0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5	pause pressed				
*/

#define ON_KEY_DOWN(scancode, x, y) \
  case scancode:                    \
    if (g_left_shift_down) {        \
      tty_putch(y);                 \
    } else {                        \
      tty_putch(x);                 \
    }                               \
    break;

#define IGNORE_KEY_DOWN(scancode, description)                        \
  case scancode:                                                      \
    LOG("ignoring key down for %02x: %s\n", scancode, description);   \
    break;

#define IGNORE_KEY_UP(scancode, description)                          \
  case scancode:                                                      \
    LOG("ignoring key up for %02x: %s\n", scancode, description);     \
    break;

static void keyboard_handler(struct isr_frame* frame) {
  switch (inb(0x60)) {
    IGNORE_KEY_DOWN(0x01, "escape");
    ON_KEY_DOWN(0x02, '1', '!');
    ON_KEY_DOWN(0x03, '2', '@');
    ON_KEY_DOWN(0x04, '3', '#');
    ON_KEY_DOWN(0x05, '4', '$');
    ON_KEY_DOWN(0x06, '5', '%');
    ON_KEY_DOWN(0x07, '6', '^');
    ON_KEY_DOWN(0x08, '7', '&');
    ON_KEY_DOWN(0x09, '8', '*');
    ON_KEY_DOWN(0x0A, '9', '(');
    ON_KEY_DOWN(0x0B, '0', ')');
    ON_KEY_DOWN(0x0C, '-', '_');
    ON_KEY_DOWN(0x0D, '=', '+');
    ON_KEY_DOWN(0x0E, '\b', '\b');
    ON_KEY_DOWN(0x0F, '\t', '\t');
    ON_KEY_DOWN(0x10, 'q', 'Q');
    ON_KEY_DOWN(0x11, 'w', 'W');
    ON_KEY_DOWN(0x12, 'e', 'E');
    ON_KEY_DOWN(0x13, 'r', 'R');
    ON_KEY_DOWN(0x14, 't', 'T');
    ON_KEY_DOWN(0x15, 'y', 'Y');
    ON_KEY_DOWN(0x16, 'u', 'U');
    ON_KEY_DOWN(0x17, 'i', 'I');
    ON_KEY_DOWN(0x18, 'o', 'O');
    ON_KEY_DOWN(0x19, 'p', 'P');
    ON_KEY_DOWN(0x1A, '[', '{');
    ON_KEY_DOWN(0x1B, ']', '}');
    ON_KEY_DOWN(0x1C, '\n', '\n');
    IGNORE_KEY_DOWN(0x1D, "left control");
    ON_KEY_DOWN(0x1E, 'a', 'A');
    ON_KEY_DOWN(0x1F, 's', 'S');
    ON_KEY_DOWN(0x20, 'd', 'D');
    ON_KEY_DOWN(0x21, 'f', 'F');
    ON_KEY_DOWN(0x22, 'g', 'G');
    ON_KEY_DOWN(0x23, 'h', 'H');
    ON_KEY_DOWN(0x24, 'j', 'J');
    ON_KEY_DOWN(0x25, 'k', 'K');
    ON_KEY_DOWN(0x26, 'l', 'L');
    ON_KEY_DOWN(0x27, ';', ':');
    ON_KEY_DOWN(0x28, '\'', '"');
    ON_KEY_DOWN(0x29, '`', '~');
    case 0x2A: g_left_shift_down = true; break;
    ON_KEY_DOWN(0x2B, '\\', '|');
    ON_KEY_DOWN(0x2C, 'z', 'Z');
    ON_KEY_DOWN(0x2D, 'x', 'X');
    ON_KEY_DOWN(0x2E, 'c', 'C');
    ON_KEY_DOWN(0x2F, 'v', 'V');
    ON_KEY_DOWN(0x30, 'b', 'B');
    ON_KEY_DOWN(0x31, 'n', 'N');
    ON_KEY_DOWN(0x32, 'm', 'M');
    ON_KEY_DOWN(0x33, ',', '<');
    ON_KEY_DOWN(0x34, '.', '>');
    ON_KEY_DOWN(0x35, '/', '?');
    IGNORE_KEY_DOWN(0x36, "right shift");
    ON_KEY_DOWN(0x37, '*', '*');
    IGNORE_KEY_DOWN(0x38, "left alt");
    ON_KEY_DOWN(0x39, ' ', ' ');
    IGNORE_KEY_DOWN(0x3A, "CapsLock");
    IGNORE_KEY_DOWN(0x3B, "F1");
    IGNORE_KEY_DOWN(0x3C, "F2");
    IGNORE_KEY_DOWN(0x3D, "F3");
    IGNORE_KEY_DOWN(0x3E, "F4");
    IGNORE_KEY_DOWN(0x3F, "F5");
    IGNORE_KEY_DOWN(0x40, "F6");
    IGNORE_KEY_DOWN(0x41, "F7");
    IGNORE_KEY_DOWN(0x42, "F8");
    IGNORE_KEY_DOWN(0x43, "F9");
    IGNORE_KEY_DOWN(0x44, "F10");
    IGNORE_KEY_DOWN(0x45, "NumberLock");
    IGNORE_KEY_DOWN(0x46, "ScrollLock");
    ON_KEY_DOWN(0x47, '7', '7');
    ON_KEY_DOWN(0x48, '8', '8');
    ON_KEY_DOWN(0x49, '9', '9');
    ON_KEY_DOWN(0x4A, '-', '-');
    ON_KEY_DOWN(0x4C, '5', '5');
    ON_KEY_DOWN(0x4B, '4', '4');
    ON_KEY_DOWN(0x4D, '6', '6');
    ON_KEY_DOWN(0x4E, '+', '+');
    ON_KEY_DOWN(0x4F, '1', '1');
    ON_KEY_DOWN(0x50, '2', '2');
    ON_KEY_DOWN(0x51, '3', '3');
    ON_KEY_DOWN(0x52, '0', '0');
    ON_KEY_DOWN(0x53, '.', '.');
    IGNORE_KEY_DOWN(0x57, "F11");
    IGNORE_KEY_DOWN(0x58, "F12");
    IGNORE_KEY_UP(0x81, "escape");
    IGNORE_KEY_UP(0x82, "1");
    IGNORE_KEY_UP(0x83, "2");
    IGNORE_KEY_UP(0x84, "3");
    IGNORE_KEY_UP(0x85, "4");
    IGNORE_KEY_UP(0x86, "5");
    IGNORE_KEY_UP(0x87, "6");
    IGNORE_KEY_UP(0x88, "7");
    IGNORE_KEY_UP(0x89, "8");
    IGNORE_KEY_UP(0x8A, "9");
    IGNORE_KEY_UP(0x8B, "0");
    IGNORE_KEY_UP(0x8C, "-");
    IGNORE_KEY_UP(0x8D, "=");
    IGNORE_KEY_UP(0x8E, "backspace");
    IGNORE_KEY_UP(0x8F, "tab");
    IGNORE_KEY_UP(0x90, "Q");
    IGNORE_KEY_UP(0x91, "W");
    IGNORE_KEY_UP(0x92, "E");
    IGNORE_KEY_UP(0x93, "R");
    IGNORE_KEY_UP(0x94, "T");
    IGNORE_KEY_UP(0x95, "Y");
    IGNORE_KEY_UP(0x96, "U");
    IGNORE_KEY_UP(0x97, "I");
    IGNORE_KEY_UP(0x98, "O");
    IGNORE_KEY_UP(0x99, "P");
    IGNORE_KEY_UP(0x9A, "[");
    IGNORE_KEY_UP(0x9B, "]");
    IGNORE_KEY_UP(0x9C, "enter");
    IGNORE_KEY_UP(0x9D, "left control");
    IGNORE_KEY_UP(0x9E, "A");
    IGNORE_KEY_UP(0x9F, "S");
    IGNORE_KEY_UP(0xA0, "D");
    IGNORE_KEY_UP(0xA1, "F");
    IGNORE_KEY_UP(0xA2, "G");
    IGNORE_KEY_UP(0xA3, "H");
    IGNORE_KEY_UP(0xA4, "J");
    IGNORE_KEY_UP(0xA5, "K");
    IGNORE_KEY_UP(0xA6, "L");
    IGNORE_KEY_UP(0xA7, ";");
    IGNORE_KEY_UP(0xA8, "' (single quote)");
    IGNORE_KEY_UP(0xA9, "` (back tick)");
    case 0xAA: g_left_shift_down = false; break;
    IGNORE_KEY_UP(0xAB, "\\");
    IGNORE_KEY_UP(0xAC, "Z");
    IGNORE_KEY_UP(0xAD, "X");
    IGNORE_KEY_UP(0xAE, "C");
    IGNORE_KEY_UP(0xAF, "V");
    IGNORE_KEY_UP(0xB0, "B");
    IGNORE_KEY_UP(0xB1, "N");
    IGNORE_KEY_UP(0xB2, "M");
    IGNORE_KEY_UP(0xB3, ",");
    IGNORE_KEY_UP(0xB4, ".");
    IGNORE_KEY_UP(0xB5, "/");
    IGNORE_KEY_UP(0xB6, "right shift");
    IGNORE_KEY_UP(0xB7, "(keypad) *");
    IGNORE_KEY_UP(0xB8, "left alt");
    IGNORE_KEY_UP(0xB9, "space");
    IGNORE_KEY_UP(0xBA, "CapsLock");
    IGNORE_KEY_UP(0xBB, "F1");
    IGNORE_KEY_UP(0xBC, "F2");
    IGNORE_KEY_UP(0xBD, "F3");
    IGNORE_KEY_UP(0xBE, "F4");
    IGNORE_KEY_UP(0xBF, "F5");
    IGNORE_KEY_UP(0xC0, "F6");
    IGNORE_KEY_UP(0xC1, "F7");
    IGNORE_KEY_UP(0xC2, "F8");
    IGNORE_KEY_UP(0xC3, "F9");
    IGNORE_KEY_UP(0xC4, "F10");
    IGNORE_KEY_UP(0xC5, "NumberLock");
    IGNORE_KEY_UP(0xC6, "ScrollLock");
    IGNORE_KEY_UP(0xC7, "(keypad) 7");
    IGNORE_KEY_UP(0xC8, "(keypad) 8");
    IGNORE_KEY_UP(0xC9, "(keypad) 9");
    IGNORE_KEY_UP(0xCA, "(keypad) -");
    IGNORE_KEY_UP(0xCB, "(keypad) 4");
    IGNORE_KEY_UP(0xCC, "(keypad) 5");
    IGNORE_KEY_UP(0xCD, "(keypad) 6");
    IGNORE_KEY_UP(0xCE, "(keypad) +");
    IGNORE_KEY_UP(0xCF, "(keypad) 1");
    IGNORE_KEY_UP(0xD0, "(keypad) 2");
    IGNORE_KEY_UP(0xD1, "(keypad) 3");
    IGNORE_KEY_UP(0xD2, "(keypad) 0");
    IGNORE_KEY_UP(0xD3, "(keypad) .");
    IGNORE_KEY_UP(0xD7, "F11");
    IGNORE_KEY_UP(0xD8, "F12");
  }
  for (struct process* proc = current_process->next; proc != current_process; proc = proc->next) {
    if (proc->state == PROCESS_IOWAIT) {
      proc->state = PROCESS_ALIVE;
    }
  }
}

void init_keyboard(void) {
    interrupt_register_handler(0x21, keyboard_handler);
}