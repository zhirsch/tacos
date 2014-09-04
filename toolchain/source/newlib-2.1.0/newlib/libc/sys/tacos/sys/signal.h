#ifndef _SYS_SIGNAL_H_
#define _SYS_SIGNAL_H_

#include <bits/signal.h>
#include <sys/types.h>

int _EXFUN(kill, (pid_t, int));
int _EXFUN(sigemptyset, (sigset_t*));
int _EXFUN(sigfillset, (sigset_t*));
int _EXFUN(sigprocmask, (int, const sigset_t*, sigset_t*));
int _EXFUN(sigsuspend, (const sigset_t*));
int _EXFUN(sigaction, (int, const struct sigaction*, struct sigaction*));

#endif /* _SYS_SIGNAL_H_ */
