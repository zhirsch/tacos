/*****************************************************************************
 * driver.c
 *****************************************************************************/

#include <tacos/driver.h>
#include <tacos/process.h>
#include <tacos/panic.h>

/**
 * Driver_Load
 *
 * Load a new driver.
 */
pid_t Driver_Load(driver_info_t *info)
{
   pid_t pid;

   /* Create a new process for the driver. */
   pid = Process_Create(info->entry_point, info->stack);

   Info("Driver %s (%d.%d.%d) loaded (pid %d).", info->name,
	info->version.major, info->version.minor, info->version.tiny, pid);

   /* Run the driver initialization function, if one is defined. */
   if (info->init) {
      (info->init)();
   }

   return pid;
}

