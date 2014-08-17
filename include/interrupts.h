#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "tss.h"

typedef void (*interrupt_handler_func)(int vector, int error_code, struct tss* prev_tss);

void init_interrupts(void);
void interrupt_register_handler(int vector, interrupt_handler_func func);

#endif /* INTERRUPTS_H */
