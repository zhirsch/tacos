#ifndef SYSCALL_INIT_H
#define SYSCALL_INIT_H

void init_syscalls(void);

#define syscall_in0()                           \
  LOG("IN:  (none)\n");

#define syscall_in1(frame, type1, arg1, fmt1)   \
  type1 arg1 = (type1)frame->ebx;               \
  LOG("IN:  %s=" fmt1 "\n", #arg1, arg1);

#define syscall_in2(frame, type1, arg1, fmt1, type2, arg2, fmt2)        \
  type1 arg1 = (type1)frame->ebx;                                       \
  type2 arg2 = (type2)frame->ecx;                                       \
  LOG("IN:  %s=" fmt1 ", %s=" fmt2 "\n", #arg1, arg1, #arg2, arg2);

#define syscall_in3(frame, type1, arg1, fmt1, type2, arg2, fmt2, type3, arg3, fmt3) \
  type1 arg1 = (type1)frame->ebx;                                       \
  type2 arg2 = (type2)frame->ecx;                                       \
  type3 arg3 = (type3)frame->edx;                                       \
  LOG("IN:  %s=" fmt1 ", %s=" fmt2 ", %s=" fmt3 "\n", #arg1, arg1, #arg2, arg2, #arg3, arg3);

#define syscall_out(frame, val, fmt)            \
  do {                                          \
    const uint32_t __out = (uint32_t)(val);     \
    LOG("OUT: " fmt "\n", __out);               \
    frame->eax = __out;                         \
  } while (0)

#endif /* SYSCALL_INIT_H */
