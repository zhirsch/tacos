#ifndef _SYS_WAIT_H_
#define _SYS_WAIT_H_

#include <sys/types.h>

#define WNOHANG         0x00000001
#define WUNTRACED       0x00000002
#define WSTOPPED        WUNTRACED
#define WEXITED         0x00000004
#define WCONTINUED      0x00000008
#define WNOWAIT         0x01000000

#define WIFEXITED(status)       (!WTERMSIG(status))
#define WEXITSTATUS(status)     (((status) >> 8) & 0xff)
#define WIFSIGNALED(status)     (!WIFSTOPPED(status) && !WIFEXITED(status))
#define WTERMSIG(status)        ((status ) & 0x7f)
#define WIFSTOPPED(status)      (((status) & 0xff) == 0x7f)
#define WSTOPSIG(status)        WEXITSTATUS(status)

pid_t _EXFUN(wait3, (int*, int, struct rusage*));

#endif /* _SYS_WAIT_H_ */
