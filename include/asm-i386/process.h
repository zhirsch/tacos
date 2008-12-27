/*****************************************************************************
 * asm/process.h
 *****************************************************************************/

/* The number of processes that can be running at one time. */
#define NUM_PROCESSES 16

/* Constant process IDs */
#define SYSTEM   0
#define MMU      1
#define HELLO    2
#define WORLD    3
#define SLEEPER  4

#define GARBAGE  (NUM_PROCESSES - 1)
