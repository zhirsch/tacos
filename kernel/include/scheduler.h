#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "tss.h"

// Yields this process's time to another process.
void scheduler_yield(void);

#endif /* SCHEDULER_H */
