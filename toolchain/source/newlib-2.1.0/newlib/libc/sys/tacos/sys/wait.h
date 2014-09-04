#ifndef _SYS_WAIT_H_
#define _SYS_WAIT_H_

#include <bits/wait.h>
#include <sys/types.h>

pid_t wait3(int* status, int options, struct rusage* rusage);

#endif /* _SYS_WAIT_H_ */
