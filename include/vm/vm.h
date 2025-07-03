#ifndef VM_H
#define VM_H

#include <stdbool.h>
#include <stdint.h>

#include "../lc3/lc3.h"

typedef struct {
  uint16_t memory[LC3_MEMORY_MAX];
  uint16_t reg[LC3_R_COUNT];
  bool running;
} vm_t;

int vm_run(const char* filename);

#endif  // VM_H
