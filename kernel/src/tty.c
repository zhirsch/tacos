#include "tty.h"

#include "log.h"
#include "mmu/heap.h"
#include "mmu/mmu.h"
#include "process.h"
#include "scheduler.h"
#include "screen.h"
#include "string.h"

#define FRAMEBUF_SIZE (10 * PAGESIZE * sizeof(uint16_t))

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

    // Prepare the input ringbuf.
    memcpy(ttys[i].input_ringbuf, "env\n", 4);
    ttys[i].input_ringbuf_start = 0;
    ttys[i].input_ringbuf_end = 4;
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
  size_t ringbuf_avail;
  if (current_process != NULL && tty->pgid != current_process->pgid) {
    PANIC("Background process tried to read from tty\n");
  }
  if (count == 0) {
    return 0;
  }
  do {
    if (tty->input_ringbuf_end >= tty->input_ringbuf_start) {
      ringbuf_avail = tty->input_ringbuf_end - tty->input_ringbuf_start;
    } else {
      ringbuf_avail = TTY_INPUT_RINGBUF_SIZE - (tty->input_ringbuf_start - tty->input_ringbuf_end);
    }
    if (ringbuf_avail == 0) {
      current_process->state = PROCESS_IOWAIT;
      scheduler_yield();
    }
  } while (ringbuf_avail == 0);
  count = min(count, ringbuf_avail);
  memcpy(buf, tty->input_ringbuf + tty->input_ringbuf_start, count);
  tty->input_ringbuf_start = (tty->input_ringbuf_start + count) % TTY_INPUT_RINGBUF_SIZE;
  return count;
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
