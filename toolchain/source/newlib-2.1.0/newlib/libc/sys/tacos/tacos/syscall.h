#ifndef TACOS_SYSCALLS_H
#define TACOS_SYSCALLS_H

#include <bits/syscall.h>
#include <errno.h>
#include <stdint.h>

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

#endif /* TACOS_SYSCALLS_H */
