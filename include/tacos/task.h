/*****************************************************************************
 * task.h
 *****************************************************************************/

#ifndef TASK_H
#define TASK_H

#include <tacos/types.h>

/* A selector for an entry into the GDT that identifies a task. */
typedef uint16_t task_selector_t;

/* A task-state segment. */
typedef struct __attribute__ ((packed))
{
   unsigned int prev_task_link : 16;
   unsigned int reserved1      : 16;

   unsigned int esp0           : 32;
   unsigned int ss0            : 16;
   unsigned int reserved2      : 16;

   unsigned int esp1           : 32;
   unsigned int ss1            : 16;
   unsigned int reserved3      : 16;

   unsigned int esp2           : 32;
   unsigned int ss2            : 16;
   unsigned int reserved4      : 16;

   unsigned int cr3            : 32;
   unsigned int eip            : 32;
   unsigned int eflags         : 32;
   unsigned int eax            : 32;
   unsigned int ecx            : 32;
   unsigned int edx            : 32;
   unsigned int ebx            : 32;
   unsigned int esp            : 32;
   unsigned int ebp            : 32;
   unsigned int esi            : 32;
   unsigned int edi            : 32;

   unsigned int es             : 16;
   unsigned int reserved5      : 16;

   unsigned int cs             : 16;
   unsigned int reserved6      : 16;

   unsigned int ss             : 16;
   unsigned int reserved7      : 16;

   unsigned int ds             : 16;
   unsigned int reserved8      : 16;

   unsigned int fs             : 16;
   unsigned int reserved9      : 16;

   unsigned int gs             : 16;
   unsigned int reserved10     : 16;

   unsigned int ldt_selector   : 16;
   unsigned int reserved11     : 16;

   unsigned int debug_trap     : 1;
   unsigned int reserved12     : 15;
   unsigned int io_map_base    : 16;
} task_state_t;

/**
 * Task_SetTaskRegister
 *
 * Loads the task-state register with a selector.
 */
static INLINE
void Task_SetTaskRegister(task_selector_t selector)
{
   __asm__ __volatile__ ("ltr %0" : : "R" (selector));
}

/**
 * Task_GetTaskRegister
 *
 * Gets the current value of the task-state regsiter.
 *
 * XXX: Create a typedef for a task-state selector.
 */
static INLINE
task_selector_t Task_GetTaskRegister(void)
{
   task_selector_t selector;
   __asm__ __volatile__ ("str %0" : "=R" (selector));
   return selector;
}

/**
 * Task_GetCurrentTask
 *
 * Get the task-state segment of the currently-executing task.
 */
extern task_state_t *Task_GetCurrentTask(void);

/**
 * Task_GetPreviousTask
 *
 * Get the task-state segment of the previous task LEVEL down the stack.
 */
extern task_state_t *Task_GetPreviousTask(unsigned int level);

/**
 * Task_GetTaskState
 *
 * Get a specific task's state.
 */
extern task_state_t *Task_GetTaskState(unsigned int index);

/* Get a specific processes's state. */
#define TASK_GetProcessTaskState(pid) Task_GetTaskState((pid))
/* Get a specific interrupt's state. */
#define TASK_GetInterruptTaskState(intr) \
   Task_GetTaskState(NUM_PROCESSES + (intr))

/**
 * Task_PrintState
 *
 * Print the current state of a task-state segment.
 */
extern void Task_PrintState(task_state_t *state);

#endif /* SEGMENTS_H */
