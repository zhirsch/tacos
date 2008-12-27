#include <tacos/panic.h>
#include <tacos/interrupts.h>
#include <tacos/kprintf.h>
#include <tacos/segments.h>

typedef struct __attribute__ ((__packed__))
{
   uint32_t edi;
   uint32_t esi;
   uint32_t ebp;
   uint32_t esp;
   uint32_t ebx;
   uint32_t edx;
   uint32_t ecx;
   uint32_t eax;
} regs_t;

static void print_regs(tss_entry_t tss)
{
   kprintf("edi = 0x%08x\n", tss.edi);
   kprintf("esi = 0x%08x\n", tss.esi);
   kprintf("ebp = 0x%08x\n", tss.ebp);
   kprintf("esp = 0x%08x\n", tss.esp);
   kprintf("ebx = 0x%08x\n", tss.ebx);
   kprintf("edx = 0x%08x\n", tss.edx);
   kprintf("ecx = 0x%08x\n", tss.ecx);
   kprintf("eax = 0x%08x\n", tss.eax);
}

static void print_state(regs_t regs, uint32_t errorcode, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   /* Get the interrupted TSS */
   tss_entry_t interrupted_tss = tss_get_previous();

   print_regs(interrupted_tss);
   kprintf("errorcode = 0x%08x\n", errorcode);
   kprintf("eip = 0x%08x\n", interrupted_tss.eip);
   kprintf("cs = 0x%04x\n", interrupted_tss.cs);
   kprintf("ss = 0x%04x\n", interrupted_tss.ss);
   kprintf("ds = 0x%04x\n", interrupted_tss.ds);
   kprintf("es = 0x%04x\n", interrupted_tss.es);
   kprintf("fs = 0x%04x\n", interrupted_tss.fs);
   kprintf("gs = 0x%04x\n", interrupted_tss.gs);
   kprintf("eflags = 0x%08x\n", interrupted_tss.eflags);
}

void intr_divide_error(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, 0, eip, cs, eflags);
   Panic("%s", "DE");
}

void intr_breakpoint(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, 0, eip, cs, eflags);
   Panic("%s", "BP");
}

void intr_overflow(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, 0, eip, cs, eflags);
   Panic("%s", "OF");
}

void intr_bound_range(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, 0, eip, cs, eflags);
   Panic("%s", "BR");
}

void intr_undefined_opcode(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, 0, eip, cs, eflags);
   Panic("%s", "UD");
}

void intr_no_math(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, 0, eip, cs, eflags);
   Panic("%s", "NM");
}

void intr_double_fault(regs_t regs, uint32_t errcode, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, errcode, eip, cs, eflags);
   Panic("%s", "DF");
}

void intr_invalid_tss(regs_t regs, uint32_t errcode, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, errcode, eip, cs, eflags);
   Panic("%s", "TS");
}

void intr_segment_not_present(regs_t regs, uint32_t errcode, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, errcode, eip, cs, eflags);
   Panic("%s", "SS");
}

void intr_general_protection(regs_t regs, uint32_t errcode, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, errcode, eip, cs, eflags);
   Panic("%s", "GP");
}

void intr_page_fault(regs_t regs, uint32_t errcode, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   uint32_t cr2;

   print_state(regs, errcode, eip, cs, eflags);
   __asm__ __volatile__ ("mov %%cr2, %0" : "=r" (cr2));
   Panic("PF, cr2 = 0x%08x\n", cr2);
}

void intr_math_fault(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, 0, eip, cs, eflags);
   Panic("%s", "MF");
}

void intr_alignment_check(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, 0, eip, cs, eflags);
   Panic("%s", "AC");
}

void intr_machine_check(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, 0, eip, cs, eflags);
   Panic("%s", "MC");
}

void intr_simd_exception(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   print_state(regs, 0, eip, cs, eflags);
   Panic("%s", "XF");
}
