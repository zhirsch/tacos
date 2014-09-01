#ifndef SYSCALL_SYSCALLS_H
#define SYSCALL_SYSCALLS_H

#include "interrupts.h"

//#define SYSCALL_chdir        1
#define SYSCALL_close        2
//#define SYSCALL_closedir     3
//#define SYSCALL_dup2         4
//#define SYSCALL_execve       5
#define SYSCALL__exit        6
#define SYSCALL_fcntl        7
//#define SYSCALL_fork         8
//#define SYSCALL_fstat        9
#define SYSCALL_getcwd      10
#define SYSCALL_getegid     11
#define SYSCALL_geteuid     12
#define SYSCALL_getgid      13
//#define SYSCALL_getgroups   14
#define SYSCALL_getpgid     15
// 16
#define SYSCALL_getpid      17
#define SYSCALL_getppid     18
#define SYSCALL_getuid      19
#define SYSCALL_isatty      20
//#define SYSCALL_kill        21
//#define SYSCALL_lseek       22
//#define SYSCALL_lstat       23
#define SYSCALL_open        24
//#define SYSCALL_opendir     25
//#define SYSCALL_pipe        26
#define SYSCALL_read        27
//#define SYSCALL_readdir     28
#define SYSCALL_sbrk        29
#define SYSCALL_setpgid     30
#define SYSCALL_sigaction   31
//#define SYSCALL_sigemptyset 32
#define SYSCALL_sigfillset  33
//#define SYSCALL_sigprocmask 34
//#define SYSCALL_sigsuspend  35
//#define SYSCALL_stat        36
#define SYSCALL_tcgetpgrp   37
#define SYSCALL_tcsetpgrp   38
//#define SYSCALL_umask       39
#define SYSCALL_waitpid     40
#define SYSCALL_write       41

void syscall_close(struct isr_frame* frame);
void syscall__exit(struct isr_frame* frame);
void syscall_fcntl(struct isr_frame* frame);
void syscall_getcwd(struct isr_frame* frame);
void syscall_getegid(struct isr_frame* frame);
void syscall_geteuid(struct isr_frame* frame);
void syscall_getgid(struct isr_frame* frame);
void syscall_getpgid(struct isr_frame* frame);
void syscall_getpid(struct isr_frame* frame);
void syscall_getppid(struct isr_frame* frame);
void syscall_getuid(struct isr_frame* frame);
void syscall_isatty(struct isr_frame* frame);
void syscall_open(struct isr_frame* frame);
void syscall_read(struct isr_frame* frame);
void syscall_sbrk(struct isr_frame* frame);
void syscall_setpgid(struct isr_frame* frame);
void syscall_sigaction(struct isr_frame* frame);
void syscall_sigfillset(struct isr_frame* frame);
void syscall_tcgetpgrp(struct isr_frame* frame);
void syscall_tcsetpgrp(struct isr_frame* frame);
void syscall_waitpid(struct isr_frame* frame);
void syscall_write(struct isr_frame* frame);

#endif /* SYSCALL_SYSCALLS_H */
