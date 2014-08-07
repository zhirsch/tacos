/*****************************************************************************
 * kprintf.c
 *****************************************************************************/

#include <tacos/kprintf.h>
#include <tacos/screen.h>
#include <lib/snprintf.h>
#include <stdarg.h>

/*****************************************************************************
 * kprintf
 *   Format and print a message to the console using a variadic argument.
 *****************************************************************************/
void kprintf(const char *format, ...)
{
   va_list ap;

   va_start(ap, format);
   kvprintf(format, ap);
   va_end(ap);
}

/*****************************************************************************
 * kvprintf
 *   Format and print a message to the console using a va_list.
 *****************************************************************************/
void kvprintf(const char *format, va_list ap)
{
   char s[512];
   int c;

   c = vsnprintf(s, sizeof(s), format, ap);
   for (int i = 0; i < c; i++)
      putch(s[i]);
}
