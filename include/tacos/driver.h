/*****************************************************************************
 * driver.h
 *****************************************************************************/

#ifndef _TACOS_DRIVER_H
#define _TACOS_DRIVER_H

#include <tacos/kernel.h>
#include <tacos/types.h>

typedef void (*driver_entry_point_func_t)(void);
typedef void (*driver_init_func_t)(void);

typedef struct
{
   const char * const              name;
   const version_t                 version;

   const driver_entry_point_func_t entry_point;
   const driver_init_func_t        init;

   uint8_t * const                 stack;
} driver_info_t;

/**
 * Driver_Load
 *
 * Load a new driver.
 */
extern pid_t Driver_Load(driver_info_t *info);

#endif /* _TACOS_DRIVER_H */
