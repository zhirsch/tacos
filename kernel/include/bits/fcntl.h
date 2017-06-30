#ifndef BITS_FCNTL_H
#define BITS_FCNTL_H

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2

#define O_ACCMODE (O_RDONLY|O_WRONLY|O_RDWR)

#define O_CREAT    00000100
#define O_EXCL     00000200
#define O_TRUNC    00001000
#define O_APPEND   00002000
#define O_NONBLOCK 00004000

#define F_DUPFD 0
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4

#define FD_CLOEXEC 1

#endif /* BITS_FCNTL_H */
