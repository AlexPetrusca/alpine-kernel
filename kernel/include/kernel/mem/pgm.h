#ifndef KERNEL_MEM_PGM_H_
#define KERNEL_MEM_PGM_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mem.h"

uint64_t pgm_init(uint64_t base, uint64_t size);
uint64_t pgm_allocate_page(uint64_t handle);
void pgm_free_page(uint64_t addr, uint64_t pgm_handle);
uint64_t pgm_total_pages(uint64_t pgm_handle);
uint64_t pgm_free_pages(uint64_t pgm_handle);
uint64_t pgm_used_pages(uint64_t pgm_handle);

int pgm_first_one(uint64_t x);

#endif //KERNEL_MEM_PGM_H_
