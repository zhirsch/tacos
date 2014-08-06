/*****************************************************************************
 * process.c
 *****************************************************************************/

#include <tacos/memory.h>
#include <tacos/panic.h>
#include <tacos/process.h>

typedef struct process_t {
   pid_t pid;
   task_state_t *state;

   /* True if this process can be run. */
   unsigned int ready : 1;
} process_t;

static process_t _processes[NUM_PROCESSES];
static process_t *_current_proc;

/**
 * _Process_New
 *
 * Initialize all necessary data structures for creating a new process.
 */
static process_t *_Process_New(void);

/**
 * Process_Create
 *
 * Create a new process. Return the PID of the new process, or -1 if the new
 * process could not be created.
 */
pid_t Process_Create(process_entry_point_t entryfunc, uint8_t *stack)
{
   process_t *proc;

   /* Get the process data structure for the new process. */
   proc = _Process_New();
   if (!proc) {
      return -1;
   }

   /* Initialize the process data structure. */
   proc->state->cr3 = Memory_GetPageTableBaseAddress();
   proc->state->eip = (uintptr_t)entryfunc;
   proc->state->eflags = 0x0012;    /* XXX: Why 0x0012? */
   proc->state->esp = (uintptr_t)stack;
   proc->state->cs = GDT_CreateSelector(1, RING0);
   proc->state->ss = GDT_CreateSelector(2, RING0);
   proc->state->ds = GDT_CreateSelector(2, RING0);
   proc->state->es = GDT_CreateSelector(2, RING0);
   proc->state->fs = GDT_CreateSelector(2, RING0);
   proc->state->gs = GDT_CreateSelector(2, RING0);

   /* Mark the new process as ready to run. */
   proc->ready = 1;

   return proc->pid;
}


/**
 * Process_Switch
 *
 * Switch to the process identified by the given PID.
 *
 * XXX: What does dpl do?
 */
void Process_Switch(pid_t pid, uint8_t dpl)
{
   process_t *_old_proc;
   uint32_t sel[2];

   Assert(pid < NUM_PROCESSES, "Attempted to switch to an invalid process: %d",
	  pid);
   Assert(dpl <= 3, "Attemped to use an invalid DPL: %d", dpl);

   Info("Jumping to process %d/%d.", pid, dpl);

   /* Save the current executing process, and set the process that is about to
      be run as the current process. */
   _old_proc = _current_proc;
   _current_proc = _processes + pid;

   /* Switch to the new process. */
   sel[0] = 0x0;
   sel[1] = PROCESS_CreateSelector(pid, dpl);
   __asm__ __volatile__ ("ljmp *(%0)" : : "r" (sel));

   /* Restore the old process to be the current process. */
   _current_proc = _old_proc;

   /* When the current process is returned to, print out a diagnostic
      message. */
   Info("Returning from process: %d/%d", pid, dpl);
}


/**
 * Process_Yield
 *
 * Give up the rest of this processes CPU time.
 */
void Process_Yield(void)
{
   pid_t next_pid;

   /* Find the first process that is ready to run. This might be the same as
      the current process. */
   next_pid = _current_proc->pid + 1;
   while (next_pid != _current_proc->pid) {
      if (_processes[next_pid].ready) {
	 break;
      }
      next_pid = (next_pid + 1) % NUM_PROCESSES;
   }

   /* Switch to the found process if it's not the current process. */
   if (next_pid != _current_proc->pid) {
      /* XXX: Shouldn't be RING0 ... */
      Process_Switch(next_pid, RING0);
   }
}


/**
 * _Process_New
 *
 * Initialize all necessary data structures for creating a new process.
 */
process_t *_Process_New(void)
{
   static pid_t _next_pid = 0;

   pid_t pid;
   process_t *proc;

   pid = _next_pid++;
   if (_processes[pid].state) {
      return NULL;
   }

   proc = _processes + pid;
   proc->pid = pid;
   proc->state = TASK_GetProcessTaskState(pid);
   return proc;
}

