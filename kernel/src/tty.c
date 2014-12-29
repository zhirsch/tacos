#include "tty.h"

#include "log.h"
#include "mmu/common.h"
#include "mmu/heap.h"
#include "process.h"
#include "screen.h"

#define FRAMEBUF_SIZE (10 * PAGESIZE * sizeof(uint16_t))

#define LOG(...) log("TTY", __VA_ARGS__)

static struct tty ttys[NUM_TTYS];
static struct tty* active_tty = NULL;

void init_tty(void) {
  for (int i = 0; i < NUM_TTYS; i++) {
    // There is no pgid associated with the tty yet.
    ttys[i].pgid = PGID_NONE;
    // Allocate the frame buffer.
    ttys[i].framebuf_pos  = 0;
    ttys[i].framebuf      = kmalloc(FRAMEBUF_SIZE);
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

int tty_write(struct tty* tty, const void* buf, size_t count) {
  if (current_process != NULL && tty->pgid != current_process->pgid) {
    // TODO: Send SIGTTOU to current_process->pgid
    LOG("Write to tty (pgid=%d) by background pgid %d\n", tty->pgid, current_process->pgid);
  }
  for (size_t i = 0; i < count; i++) {
    const char ch = ((char*)buf)[i];
    tty->framebuf[tty->framebuf_pos + i] = ch;
    if (tty == active_tty) {
      screen_writech(ch);
    }
    tty->framebuf_pos++;
    tty->framebuf_pos %= FRAMEBUF_SIZE;
  }
  return count;
}
