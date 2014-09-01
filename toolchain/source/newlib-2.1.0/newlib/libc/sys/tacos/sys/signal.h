#ifndef _SYS_SIGNAL_H_
#define _SYS_SIGNAL_H_

#include <sys/types.h>
#include <sys/signum.h>

typedef void (*_sig_func_ptr) (int);
typedef _sig_func_ptr __sighandler_t;

typedef unsigned long sigset_t;

typedef struct {
  int si_signo;
  int si_code;
} siginfo_t;

struct sigaction {
  void (*sa_handler)(int);
  sigset_t sa_mask;
  int sa_flags;
  void (*sa_sigaction)(int, siginfo_t*, void*);
};

#define SIG_SETMASK 0
#define SIG_BLOCK   1

int _EXFUN(kill, (pid_t, int));
int _EXFUN(sigemptyset, (sigset_t*));
int _EXFUN(sigfillset, (sigset_t*));
int _EXFUN(sigprocmask, (int, const sigset_t*, sigset_t*));
int _EXFUN(sigsuspend, (const sigset_t*));
int _EXFUN(sigaction, (int, const struct sigaction*, struct sigaction*));

#endif /* _SYS_SIGNAL_H_ */
