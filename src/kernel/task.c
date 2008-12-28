#include <tacos/task.h>
#include <tacos/gdt.h>
#include <tacos/kprintf.h>

/* The task-state segment. This is defined in assembly. */
extern task_state_t tss[];


/**
 * Task_GetCurrentTask
 *
 * Get the task-state segment of the currently-executing task.
 */
task_state_t *Task_GetCurrentTask(void)
{
   /* Get the 0th previous task (which is the current task). */
   return Task_GetPreviousTask(0);
}


/**
 * Task_GetPreviousTask
 *
 * Get the task-state segment of the previous task LEVEL down the stack.
 */
task_state_t *Task_GetPreviousTask(unsigned int level)
{
   /* This code is wrong. Jumping to a new task does not set the previous
    * task link field in the TSS.
    */
   gdt_task_entry_t *entry;
   task_selector_t selector;
   uintptr_t addr;

   /* Get the current task's selector. */
   selector = Task_GetTaskRegister();

   while (level > 1) {
      task_state_t *state;

      /* Compute the address of the task segment. First, get the GDT entry for
	 the task, then join the fields in the GDT to get the address. */
      entry = Gdt_EntryFromSelector(selector);
      addr = (entry->base_hi << 24) | (entry->base_mid << 16) | entry->base_lo;
      state = (task_state_t *)addr;

      /* Get the previous task's selector to move down the state one level. */
      selector = state->prev_task_link;
   }

   entry = Gdt_EntryFromSelector(selector);
   addr = (entry->base_hi << 24) | (entry->base_mid << 16) | entry->base_lo;
   return (task_state_t *)addr;
}


/**
 * Task_GetTaskState
 *
 * Get a specific task's state, identified by its PID.
 */
task_state_t *Task_GetTaskState(unsigned int index)
{
   return tss + index;
}


/**
 * Task_PrintState
 *
 * Print the current state of a task-state segment.
 */
void Task_PrintState(task_state_t *state)
{
   kprintf("   edi = 0x%08x\n", state->edi);
   kprintf("   esi = 0x%08x\n", state->esi);
   kprintf("   ebp = 0x%08x\n", state->ebp);
   kprintf("   esp = 0x%08x\n", state->esp);
   kprintf("   ebx = 0x%08x\n", state->ebx);
   kprintf("   edx = 0x%08x\n", state->edx);
   kprintf("   ecx = 0x%08x\n", state->ecx);
   kprintf("   eax = 0x%08x\n", state->eax);
   kprintf("   eip = 0x%08x\n", state->eip);
   kprintf("    cs = 0x%04x\n", state->cs);
   kprintf("    ss = 0x%04x\n", state->ss);
   kprintf("    ds = 0x%04x\n", state->ds);
   kprintf("    es = 0x%04x\n", state->es);
   kprintf("    fs = 0x%04x\n", state->fs);
   kprintf("    gs = 0x%04x\n", state->gs);
   kprintf("eflags = 0x%08x\n", state->eflags);
}

