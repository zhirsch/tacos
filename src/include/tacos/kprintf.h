/*****************************************************************************
 * kprintf.h
 *****************************************************************************/

#ifndef KPRINTF_H
#define KPRINTF_H

#include <stdarg.h>

/*****************************************************************************
 * kprintf
 *   Format and print a message to the console using a variadic argument.
 *****************************************************************************/
extern void kprintf(const char *format, ...)
   __attribute__ ((format(printf, 1, 2)));

/*****************************************************************************
 * kvprintf
 *   Format and print a message to the console using a va_list.
 *****************************************************************************/
extern void kvprintf(const char *format, va_list ap);

#endif /* KPRINTF_H */
