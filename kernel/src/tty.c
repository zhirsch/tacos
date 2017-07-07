#include "tty.h"

#include "log.h"
#include "mmu/heap.h"
#include "mmu/mmu.h"
#include "process.h"
#include "scheduler.h"
#include "screen.h"
#include "string.h"

#define FRAMEBUF_SIZE (10 * PAGESIZE * sizeof(uint16_t))
#define INPUTBUF_SIZE PAGESIZE

#define LOG(...) log("TTY", __VA_ARGS__)
#define PANIC(...) log("TTY", __VA_ARGS__)

static struct tty ttys[NUM_TTYS];
static struct tty* active_tty = NULL;

void init_tty(void) {
  for (int i = 0; i < NUM_TTYS; i++) {
    // There is no pgid associated with the tty yet.
    ttys[i].pgid = PGID_NONE;

    // Allocate the frame buffer.
    ttys[i].framebuf_pos  = 0;
    ttys[i].framebuf      = kmalloc(FRAMEBUF_SIZE);

    // Allocate the input buffer.
    ttys[i].inputbuf_pos = 0;
    ttys[i].inputbuf     = kmalloc(INPUTBUF_SIZE);
  }
  // The initial active TTY is arbitrarily the first TTY.
  active_tty = &ttys[0];
}

struct tty* tty_get(int i) {
  return &ttys[i];
}

void tty_setpgid(struct tty* tty, pid_t pgid) {
  tty->pgid = pgid;
}

static size_t min(size_t lhs, size_t rhs) {
  return (lhs < rhs) ? lhs : rhs;
}

int tty_read(struct tty* tty, void* buf, size_t count) {
  if (current_process != NULL && tty->pgid != current_process->pgid) {
    PANIC("Background process tried to read from tty\n");
  }
  if (count == 0) {
    return 0;
  }
  while (tty->inputbuf_pos == 0) {
    current_process->state = PROCESS_IOWAIT;
    scheduler_yield();
  }
  count = min(count, tty->inputbuf_pos);
  memcpy(buf, tty->inputbuf, count);
  tty->inputbuf_pos -= count;
  return count;
}

static void add_to_framebuf(struct tty* tty, char ch) {
  tty->framebuf[tty->framebuf_pos] = ch;
  if (tty == active_tty) {
    screen_writech(ch);
  }
  tty->framebuf_pos++;
  tty->framebuf_pos %= FRAMEBUF_SIZE;
}

int tty_write(struct tty* tty, const void* buf, size_t count) {
  if (current_process != NULL && tty->pgid != current_process->pgid) {
    // TODO: Send SIGTTOU to current_process->pgid
    LOG("Write to tty (pgid=%d) by background pgid %d\n", tty->pgid, current_process->pgid);
  }
  for (size_t i = 0; i < count; i++) {
    const char ch = ((char*)buf)[i];
    add_to_framebuf(tty, ch);
  }
  return count;
}

void tty_putch(char ch) {
  active_tty->inputbuf[active_tty->inputbuf_pos++] = ch;
  add_to_framebuf(active_tty, ch);
}