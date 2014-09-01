INCLUDES = $(NEWLIB_CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS)
AM_CCASFLAGS = $(INCLUDES)

noinst_LIBRARIES = lib.a

if MAY_SUPPLY_SYSCALLS
extra_objs = \
	$(lbfx)_exit.o       \
	$(lbfx)chdir.o       \
	$(lbfx)close.o       \
	$(lbfx)closedir.o    \
	$(lbfx)dup2.o        \
	$(lbfx)execve.o      \
	$(lbfx)fcntl.o       \
	$(lbfx)fork.o        \
	$(lbfx)fstat.o       \
	$(lbfx)getcwd.o      \
	$(lbfx)getegid.o     \
	$(lbfx)geteuid.o     \
	$(lbfx)getgid.o      \
	$(lbfx)getgroups.o   \
	$(lbfx)getpgid.o     \
	$(lbfx)getpgrp.o     \
	$(lbfx)getpid.o      \
	$(lbfx)getppid.o     \
	$(lbfx)getuid.o      \
	$(lbfx)isatty.o      \
	$(lbfx)kill.o        \
	$(lbfx)lseek.o       \
	$(lbfx)lstat.o       \
	$(lbfx)open.o        \
	$(lbfx)opendir.o     \
	$(lbfx)pipe.o        \
	$(lbfx)read.o        \
	$(lbfx)readdir.o     \
	$(lbfx)sbrk.o        \
	$(lbfx)setpgid.o     \
	$(lbfx)sigaction.o   \
	$(lbfx)sigemptyset.o \
	$(lbfx)sigfillset.o  \
	$(lbfx)sigprocmask.o \
	$(lbfx)sigsuspend.o  \
	$(lbfx)stat.o        \
	$(lbfx)tcgetpgrp.o   \
	$(lbfx)tcsetpgrp.o   \
	$(lbfx)umask.o       \
	$(lbfx)wait3.o       \
	$(lbfx)waitpid.o     \
	$(lbfx)write.o
else
extra_objs =
endif

lib_a_SOURCES =
lib_a_LIBADD = $(extra_objs)
EXTRA_lib_a_SOURCES = \
	_exit.c       \
	chdir.c       \
	close.c       \
	closedir.c    \
	crt0.S        \
	dup2.c        \
	execve.c      \
	fcntl.c       \
	fork.c        \
	fstat.c       \
	getcwd.c      \
	getegid.c     \
	geteuid.c     \
	getgid.c      \
	getgroups.c   \
	getpgid.c     \
	getpgrp.c     \
	getpid.c      \
	getppid.c     \
	getuid.c      \
	isatty.c      \
	kill.c        \
	lseek.c       \
	lstat.c       \
	open.c        \
	opendir.c     \
	pipe.c        \
	read.c        \
	readdir.c     \
	sbrk.c        \
	setpgid.c     \
	sigaction.c   \
	sigemptyset.c \
	sigfillset.c  \
	sigprocmask.c \
	sigsuspend.c  \
	stat.c        \
	tcgetpgrp.c   \
	tcsetpgrp.c   \
	umask.c       \
	wait3.c       \
	waitpid.c     \
	write.c
lib_a_DEPENDENCIES = $(extra_objs)
lib_a_CCASFLAGS = $(AM_CCASFLAGS)
lib_a_CFLAGS = $(AM_CFLAGS)

if MAY_SUPPLY_SYSCALLS
all: crt0.o
endif

ACLOCAL_AMFLAGS = -I ../../..
CONFIG_STATUS_DEPENDENCIES = $(newlib_basedir)/configure.host
