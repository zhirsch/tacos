#ifndef RING3_H
#define RING3_H

// Switches the process to execute in ring 3.
// TODO(zhirsch): Allow the address to start execution at in ring 3 to be
// passed.
void switch_to_ring3(void* entry);

#endif /* RING3_H */
