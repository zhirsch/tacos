/*****************************************************************************
 * process.c
 *****************************************************************************/

#include <tacos/process.h>
#include <tacos/types.h>
#include <tacos/segments.h>
#include <tacos/panic.h>

extern tss_entry_t tss[];

/*****************************************************************************
 * process_switch
 *   Switch to the task identified by the given PID.
 *****************************************************************************/
void process_switch(pid_t pid, uint8_t dpl)
{
   Assert(pid < N_PROCESSES, "Attempted to jump to an invalid process: %d", pid);
   Assert(dpl <= 3, "Attemped to use an invalid DPL: %d", dpl);

   unsigned int sel[2];
   sel[0] = 0x0;
   sel[1] = GDT_SELECTOR(GDT_TSS_BASE + pid, dpl);

   Info("Jumping to process pid = %d, pl = %d", pid, dpl);
   Info("  cr3 = %08x, eip = %08x, esp = %08x", tss[pid].cr3, tss[pid].eip, tss[pid].esp);
   Info("  cs = %x, ds = %x, ss =%x", tss[pid].cs, tss[pid].ds, tss[pid].ss);
   __asm__ __volatile__ ("ljmp *(%0)" : : "p" (sel));
   Info("Returning to process after having switched to process: %x/%d", pid, dpl);
}
