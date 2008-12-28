/*****************************************************************************
 * process.h
 *   Contains functions and macros for the process manager.
 *****************************************************************************/

#ifndef PROCESS_H
#define PROCESS_H

#include <asm/process.h>
#include <tacos/types.h>
#include <tacos/gdt.h>

typedef void (*process_entry_point_t)(void);

/* Gets the GDT selector for a process's task segment. */
#define PROCESS_CreateSelector(x, y) \
   GDT_CreateSelector(GDT_TASK_BASE + (x), (y))

/**
 * Process_Create
 *
 * Create a new process.
 */
extern pid_t Process_Create(process_entry_point_t entryfunc, uint8_t *stack);

/**
 * Process_Switch
 *
 * Switch to the process identified by the given PID.
 *
 * XXX: What does dpl do?
 */
extern void Process_Switch(pid_t pid, uint8_t dpl);

/**
 * Process_Yield
 *
 * Give up the rest of this processes CPU time.
 */
extern void Process_Yield(void);

#endif /* PROCESS_H */
