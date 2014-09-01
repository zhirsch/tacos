#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()      \
  do {                                \
    builtin_define_std ("tacos");     \
    builtin_define_std ("unix");      \
    builtin_assert ("system=tacos");  \
    builtin_assert ("system=unix");   \
  } while(0);
