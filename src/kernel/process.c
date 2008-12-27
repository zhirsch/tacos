/*****************************************************************************
 * process.c
 *****************************************************************************/

#include <tacos/process.h>
#include <tacos/task.h>
#include <tacos/panic.h>

/*****************************************************************************
 * Process_Switch
 *   Switch to the process identified by the given PID.
 *
 * XXX: What does dpl do?
 *****************************************************************************/
void Process_Switch(pid_t pid, uint8_t dpl)
{
   task_state_t *state;
   unsigned int sel[2];

   Assert(pid < NUM_PROCESSES, "Attempted to switch to an invalid process: %d",
	  pid);
   Assert(dpl <= 3, "Attemped to use an invalid DPL: %d", dpl);

   /* Create the selector to jump to. */
   sel[0] = 0x0;
   sel[1] = PROCESS_CreateSelector(pid, dpl);

   /* Get the state of the process that's about to be switched to, so that some
      diagnostic information can be printed. */
   state = TASK_GetProcessTaskState(pid);
   Info("Jumping to process %d/%d", pid, dpl);
   Info(" cr3=%08x, eip=%08x, esp=%08x", state->cr3, state->eip, state->esp);
   Info(" cs=%x, ds=%x, ss=%x", state->cs, state->ds, state->ss);

   /* Switch to the new process. */
   __asm__ __volatile__ ("ljmp *(%0)" : : "p" (sel));

   /* When the current process is returned to, print out a diagnostic
      message. */
   Info("Returning from process: %d/%d", pid, dpl);
}

