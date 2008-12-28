/*****************************************************************************
 * process.c
 *****************************************************************************/

#include <tacos/memory.h>
#include <tacos/panic.h>
#include <tacos/process.h>

typedef struct process_t {
   pid_t pid;
   task_state_t *state;
} process_t;

static process_t _processes[NUM_PROCESSES];
static pid_t _next_pid = 0;

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
pid_t Process_Create(process_entry_point_t entryfunc, uint8_t *stack, int mode)
{
   process_t *proc;

   /* Get the process data structure for the new process. */
   proc = _Process_New();
   if (!proc) {
      return -1;
   }

   /* Initialize the process data structure. */
   /*
   proc->state->esp0 = (uintptr_t)stack;
   proc->state->ss0 = GDT_CreateSelector(2, RING0);
   */
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
   task_state_t *state;
   uint32_t sel[2];

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
   __asm__ __volatile__ ("ljmp *(%0)" : : "r" (sel));

   /* When the current process is returned to, print out a diagnostic
      message. */
   Info("Returning from process: %d/%d", pid, dpl);
}


/**
 * _Process_New
 *
 * Initialize all necessary data structures for creating a new process.
 */
process_t *_Process_New(void)
{
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

