/*****************************************************************************
 * panic.c
 *****************************************************************************/

#include <tacos/panic.h>
#include <tacos/kprintf.h>
#include <tacos/types.h>
#include <stdarg.h>

static void Log(const char *type, const char *file, uint32_t line,
		const char *format, va_list ap)
{
   kprintf("%s (%s:%d): ", type, file, line);
   kvprintf(format, ap);
   kprintf("\n");   
}

void _Info(const char *file, uint32_t line, const char *format, ...)
{
   va_list ap;

   va_start(ap, format);
   Log("INFO ", file, line, format, ap);
   va_end(ap);
}

void _Panic(const char *file, uint32_t line, const char *format, ...)
{
   va_list ap;

   va_start(ap, format);
   Log("PANIC", file, line, format, ap);
   va_end(ap);

   /* Disable interrupts and halt the CPU */
   __asm__ __volatile__ ("cli");
   __asm__ __volatile__ ("hlt");

   while (1); // This function is declared as noreturn
}
