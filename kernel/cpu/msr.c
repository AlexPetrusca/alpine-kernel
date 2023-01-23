#include <kernel/cpu/msr.h>
#include <kernel/cpu/asm.h>

void msr_read(uint32_t msr, uint32_t* lo, uint32_t* hi) {
  RDMSR(msr, *lo, *hi);
}

uint64_t msr_read64(uint32_t msr) {
  uint32_t lo, hi;
  msr_read(msr, &lo, &hi);
  return (uint64_t) lo + ((uint64_t) hi << 32);
}

void msr_write(uint32_t msr, uint32_t lo, uint32_t hi) {
  WRMSR(msr, lo, hi);
}

void msr_write64(uint32_t msr, uint64_t value) {
  msr_write(msr, value & 0xFFFFFFFF, (value >> 32) & 0xFFFFFFFF);
}