/*****************************************************************************
 * asm/process.h
 *****************************************************************************/

#include <asm/segments.h>

/* The number of processes that can be running at one time. */
#define N_PROCESSES 16

/* Gets the GDT selector for a process's task segment. */
#define PROCESS_TSS(x, y) GDT_SELECTOR(GDT_TSS_BASE + (x), (y))

/* Constant process IDs */
#define SYSTEM   0
#define MMU      1
#define GARBAGE (N_PROCESSES - 1)
