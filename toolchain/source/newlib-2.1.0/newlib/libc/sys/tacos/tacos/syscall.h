#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <errno.h>
#include <stdint.h>

#define __SYSCALL_chdir        1
#define __SYSCALL_close        2
#define __SYSCALL_closedir     3
#define __SYSCALL_dup2         4
#define __SYSCALL_execve       5
#define __SYSCALL__exit        6
#define __SYSCALL_fcntl        7
#define __SYSCALL_fork         8
#define __SYSCALL_fstat        9
#define __SYSCALL_getcwd      10
#define __SYSCALL_getegid     11
#define __SYSCALL_geteuid     12
#define __SYSCALL_getgid      13
#define __SYSCALL_getgroups   14
#define __SYSCALL_getpgid     15
/* #define __SYSCALL_getpgrp     16 */
#define __SYSCALL_getpid      17
#define __SYSCALL_getppid     18
#define __SYSCALL_getuid      19
#define __SYSCALL_isatty      20
#define __SYSCALL_kill        21
#define __SYSCALL_lseek       22
#define __SYSCALL_lstat       23
#define __SYSCALL_open        24
#define __SYSCALL_opendir     25
#define __SYSCALL_pipe        26
#define __SYSCALL_read        27
#define __SYSCALL_readdir     28
#define __SYSCALL_sbrk        29
#define __SYSCALL_setpgid     30
#define __SYSCALL_sigaction   31
#define __SYSCALL_sigemptyset 32
#define __SYSCALL_sigfillset  33
#define __SYSCALL_sigprocmask 34
#define __SYSCALL_sigsuspend  35
#define __SYSCALL_stat        36
#define __SYSCALL_tcgetpgrp   37
#define __SYSCALL_tcsetpgrp   38
#define __SYSCALL_umask       39
#define __SYSCALL_waitpid     40
#define __SYSCALL_write       41

// The following are inspired by Linux.
#define __syscall_return(type, res)                           \
  do {                                                        \
    if ((unsigned long)(res) >= (unsigned long)(-125)) {      \
      errno = -(res);                                         \
      res = -1;                                               \
    }                                                         \
    return (type)(res);                                       \
  } while (0)

// Inline assembly for performing syscalls with a fixed number of arguments.
#define __inline_syscall0(name, ret)                    \
  __asm__ __volatile__ ("int $0xFF"                     \
                        : "=a" (ret)                    \
                        : "0" (__SYSCALL_##name))
#define __inline_syscall1(name, ret, arg1)                      \
  __asm__ __volatile__ ("int $0xFF"                             \
                        : "=a" (ret)                            \
                        : "0" (__SYSCALL_##name),               \
                          "b" (arg1));
#define __inline_syscall2(name, ret, arg1, arg2)                \
  __asm__ __volatile__ ("int $0xFF"                             \
                        : "=a" (ret)                            \
                        : "0" (__SYSCALL_##name),               \
                          "b" (arg1),                           \
                          "c" (arg2));
#define __inline_syscall3(name, ret, arg1, arg2, arg3)          \
  __asm__ __volatile__ ("int $0xFF"                             \
                        : "=a" (ret)                            \
                        : "0" (__SYSCALL_##name),               \
                          "b" (arg1),                           \
                          "c" (arg2),                           \
                          "d" (arg3));

// Function defintions for a syscall with a given name.
#define _syscall0(type, name)                   \
  type name(void) {                             \
    long __res;                                 \
    __inline_syscall0(name, __res);             \
    __syscall_return(type, __res);              \
  }
#define _syscall1(type, name, type1, arg1)      \
  type name(type1 arg1) {                       \
    long __res;                                 \
    __inline_syscall1(name, __res, arg1);       \
    __syscall_return(type, __res);              \
  }
#define _syscall2(type, name, type1, arg1, type2, arg2) \
  type name(type1 arg1, type2 arg2) {                   \
    long __res;                                         \
    __inline_syscall2(name, __res, arg1, arg2);         \
    __syscall_return(type, __res);                      \
  }
#define _syscall3(type, name, type1, arg1, type2, arg2, type3, arg3)    \
  type name(type1 arg1, type2 arg2, type3 arg3) {                       \
    long __res;                                                         \
    __inline_syscall3(name, __res, arg1, arg2, arg3);                   \
    __syscall_return(type, __res);                                      \
  }

#endif /* _SYSCALLS_H_ */
