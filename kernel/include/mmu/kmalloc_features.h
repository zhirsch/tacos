#ifndef KMALLOC_FEATURES_H
#define KMALLOC_FEATURES_H

#include <stddef.h>
#include <stdint.h>

#include "log.h"
#include "mmu/linear.h"

// Rename all the public functions to start with a "k".
#define USE_DL_PREFIX
#define dlcalloc                     kcalloc
#define dlfree                       kfree
#define dlmalloc                     kmalloc
#define dlmemalign                   kmemalign
#define dlposix_memalign             kposix_memalign
#define dlrealloc                    krealloc
#define dlrealloc_in_place           krealloc_in_place
#define dlvalloc                     kvalloc
#define dlpvalloc                    kpvalloc
#define dlmallinfo                   kmallinfo
#define dlmallopt                    kmallopt
#define dlmalloc_trim                kmalloc_trim
#define dlmalloc_stats               kmalloc_stats
#define dlmalloc_usable_size         kmalloc_usable_size
#define dlmalloc_footprint           kmalloc_footprint
#define dlmalloc_max_footprint       kmalloc_max_footprint
#define dlmalloc_footprint_limit     kmalloc_footprint_limit
#define dlmalloc_set_footprint_limit kmalloc_set_footprint_limit
#define dlmalloc_inspect_all         kmalloc_inspect_all
#define dlindependent_calloc         kindependent_calloc
#define dlindependent_comalloc       kindependent_comalloc
#define dlbulk_free                  kbulk_free

// The kernel doesn't provide mmap.
#define HAVE_MMAP 0

// The kernel doesn't provide some headers.
#define LACKS_ERRNO_H
#define LACKS_STDLIB_H
#define LACKS_STRING_H
#define LACKS_SYS_TYPES_H
#define LACKS_TIME_H
#define LACKS_UNISTD_H

// Don't provide malloc stats.
#define NO_MALLOC_STATS 1

// Statically define the page size.
#define malloc_getpagesize PAGESIZE

// Define functions that kmalloc calls.
#define ABORT                 panic("KMALLOC", "failed\n");
#define MALLOC_FAILURE_ACTION { } /* Do nothing */
#define MORECORE              lmmu_sbrk

#endif /* KMALLOC_FEATURES_H */
