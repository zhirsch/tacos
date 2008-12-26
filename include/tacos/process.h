/*****************************************************************************
 * process.h
 *   Contains functions and macros for the process manager.
 *****************************************************************************/

#ifndef PROCESS_H
#define PROCESS_H

#include <asm/process.h>
#include <tacos/types.h>

/*****************************************************************************
 * process_switch
 *   Switch to the process identified by the given PID.
 *****************************************************************************/
extern void process_switch(pid_t pid, uint8_t dpl);

/*****************************************************************************
 * process_ltr
 *   Loads the task-state register with a selector for a process.
 *****************************************************************************/
static INLINE
void process_ltr(uint16_t selector)
{
   __asm__ __volatile__ ("ltr %0" : : "R" (selector));
}

#endif /* PROCESS_H */
