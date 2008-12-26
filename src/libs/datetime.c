/*****************************************************************************
 * datetime.c
 *   Implements some basic time functions.
 *****************************************************************************/

#include <lib/datetime.h>
#include <tacos/panic.h>

extern int getsecond(void);

static int bcd2bin(int x)
{
   int accum = 0;
   int factor = 1;

   while (x > 0) {
      accum += (x & 0xF) * factor;
      factor *= 10;
      x >>= 4;
   }

   return accum;
}

/*****************************************************************************
 * sleep
 *   Sleeps (doesn't return) for a certain number of seconds.
 *
 * This supports only a maximum sleep time of 59 seconds!
 *
 * XXX: This should probably be rewritten using the PIT at some point.
 *****************************************************************************/
void sleep(int nsecs)
{
   Assert(nsecs < 60, "%s", "Can't sleep for that long!");

   int cursecs = bcd2bin(getsecond());
   int end = (cursecs + nsecs) % 60;

   int last = cursecs;

   while (cursecs != end) {
      if (cursecs != last) {
         last = cursecs;
      }
      cursecs = bcd2bin(getsecond());
   }
}
