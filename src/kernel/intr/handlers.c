#include <tacos/interrupt.h>
#include <tacos/task.h>
#include <tacos/gdt.h>
#include <tacos/panic.h>
#include <tacos/kprintf.h>

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

static void _PrintPreviousTaskState(void)
{
   Task_PrintState(Task_GetPreviousTask(1));
}

void intr_divide_error(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   Panic("%s", "DE");
}

void intr_breakpoint(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   Panic("%s", "BP");
}

void intr_overflow(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   Panic("%s", "OF");
}

void intr_bound_range(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   Panic("%s", "BR");
}

void intr_undefined_opcode(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   Panic("%s", "UD");
}

void intr_no_math(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   Panic("%s", "NM");
}

void intr_double_fault(regs_t regs, uint32_t errcode, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   kprintf("errcode: 0x%08x\n", errcode);
   Panic("%s", "DF");
}

void intr_invalid_tss(regs_t regs, uint32_t errcode, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   kprintf("errcode: 0x%08x\n", errcode);
   Panic("%s", "TS");
}

void intr_segment_not_present(regs_t regs, uint32_t errcode, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   kprintf("errcode: 0x%08x\n", errcode);
   Panic("%s", "SS");
}

void intr_general_protection(regs_t regs, uint32_t errcode, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   kprintf("errcode: 0x%08x\n", errcode);
   Panic("%s", "GP");
}

void intr_page_fault(regs_t regs, uint32_t errcode, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   uint32_t cr2;

   _PrintPreviousTaskState();
   kprintf("errcode: 0x%08x\n", errcode);
   __asm__ __volatile__ ("mov %%cr2, %0" : "=r" (cr2));
   Panic("PF, cr2 = 0x%08x\n", cr2);
}

void intr_math_fault(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   Panic("%s", "MF");
}

void intr_alignment_check(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   Panic("%s", "AC");
}

void intr_machine_check(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   Panic("%s", "MC");
}

void intr_simd_exception(regs_t regs, uint32_t eip, uint32_t cs, uint32_t eflags)
{
   _PrintPreviousTaskState();
   Panic("%s", "XF");
}
