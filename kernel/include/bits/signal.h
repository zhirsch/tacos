#ifndef BITS_SIGNAL_H
#define BITS_SIGNAL_H

#include <stdint.h>

#define SIGHUP   1
#define SIGINT   2
#define SIGQUIT  3
#define SIGABRT  6
#define SIGKILL  9
#define SIGPIPE 13
#define SIGTERM 15
#define SIGCHLD 17
#define SIGCONT 18
#define SIGSTOP 19
#define SIGTSTP 20
#define SIGTTIN 21
#define SIGTTOU 22

#define NSIG 32

#define SIG_SETMASK 0
#define SIG_BLOCK   1

typedef void (*_sig_func_ptr) (int);
typedef _sig_func_ptr __sighandler_t;

typedef uint32_t sigset_t;

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

#endif /* BITS_SIGNAL_H */
