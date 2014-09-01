#ifndef SYSCALL_SYSCALLS_H
#define SYSCALL_SYSCALLS_H

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
// 16
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

#endif /* SYSCALL_SYSCALLS_H */
