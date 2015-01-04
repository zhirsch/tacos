#ifndef LDSYMS_H
#define LDSYMS_H

// Symbols defined in the linker script.

// TODO: Extract these directly from the linker script.  Or generate the linker
// script and header file with a script.

extern const void kernel_origin;

extern const void kernel_start;
extern const void kernel_end;

extern const void kernel_ro_start;
extern const void kernel_ro_end;

extern const void kernel_rw_start;
extern const void kernel_rw_end;

extern const void kernel_heap_start;
extern const void kernel_heap_end;

#endif /* LDSYMS_H */
