/*****************************************************************************
 * snprintf.h
 *****************************************************************************/

#ifndef SNPRINTF_H
#define SNPRINTF_H

#include <tacos/types.h>
#include <stdarg.h>

extern int snprintf(char *str, size_t size, const char *format, ...);
extern int vsnprintf(char *str, size_t size, const char *format, va_list ap);

#endif /* SNPRINTF_H */
