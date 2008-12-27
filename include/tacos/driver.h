/*****************************************************************************
 * driver.h
 *****************************************************************************/

#ifndef _TACOS_DRIVER_H
#define _TACOS_DRIVER_H

#include <tacos/kernel.h>
#include <tacos/types.h>

typedef void (*driver_entry_point_func_t)(void);
typedef void (*driver_init_func_t)(void);
typedef uint8_t *driver_stack_t;

typedef struct
{
   const char * const              name;
   const version_t                 version;
   const driver_entry_point_func_t entry_point_func;
   const driver_init_func_t        init_func;
   const driver_stack_t            stack;
} driver_info_t;

#endif /* _TACOS_DRIVER_H */
