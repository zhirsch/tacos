/*****************************************************************************
 * panic.h
 *****************************************************************************/

#ifndef PANIC_H
#define PANIC_H

#include <tacos/types.h>

/*****************************************************************************
 * Info
 *   Print out a message.
 *****************************************************************************/
#define Info(format, ...) _Info(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define Info0(format) Info("%s", format);
extern void _Info(const char *file, uint32_t line, const char *format, ...)
   __attribute__ ((format(printf, 3, 4)));

/*****************************************************************************
 * Panic
 *   Print out a message and halt the CPU.
 *****************************************************************************/
#define Panic(format, ...) _Panic(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define Panic0(format) Panic("%s", format)
extern void _Panic(const char *file, uint32_t line, const char *format, ...)
   __attribute__ ((noreturn, format(printf, 3, 4)));

/* Test a condition and Panic if it is false. */
#define Assert(cond, msg, ...) __extension__			\
   ({								\
      if (!(cond))						\
      {								\
	 _Panic(__FILE__, __LINE__, msg, ##__VA_ARGS__);	\
      }								\
   })

#endif /* PANIC_H */
