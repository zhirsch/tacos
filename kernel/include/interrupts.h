#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

struct isr_frame {
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  int vector;
  int error_code;
  uint32_t user_eip;
  uint32_t user_cs;
  uint32_t user_eflags;
  uint32_t user_esp;
  uint32_t user_ss;
} __attribute__ ((packed));

typedef void (*interrupt_handler_func)(struct isr_frame* frame);

void init_interrupts(void);
void interrupt_register_handler(int vector, interrupt_handler_func func);

#endif /* INTERRUPTS_H */
