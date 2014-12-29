#include "mmu/heap.h"

#include <stddef.h>

#include "log.h"
#include "mmu/common.h"
#include "string.h"

#define PANIC(...) panic("HEAP", __VA_ARGS__)
#define LOG(...) log("HEAP", __VA_ARGS__)

static uintptr_t heap = (uintptr_t)LDSYM_LADDR(kernel_heap_start);
static uintptr_t kHeapEnd = (uintptr_t)LDSYM_LADDR(kernel_heap_end);

void* kmalloc(size_t size) {
  const uintptr_t ptr = heap;
  if (size == 0) {
    return NULL;
  }
  if (ptr + size > kHeapEnd) {
    PANIC("kmalloc(%ld) out of heap\n", size);
  }
  while (heap < ptr + size) {
    mmu_map_system_rw_page((void*)heap);
    heap += PAGESIZE;
  }
  return (void*)ptr;
}

void* kcalloc(size_t nmemb, size_t size) {
  void* ptr = kmalloc(nmemb * size);
  if (ptr == NULL) {
    return NULL;
  }
  memset(ptr, 0, nmemb * size);
  return ptr;
}

void kfree(void* ptr) {
  // TODO: implement
}
