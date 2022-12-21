#include <kernel/msr.h>

void readMSR(uint32_t msr, uint32_t* lo, uint32_t* hi) {
  __asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

uint64_t readMSR64(uint32_t msr) {
  uint32_t lo, hi;
  readMSR(msr, &lo, &hi);
  return (uint64_t) lo + ((uint64_t) hi << 32);
}

void writeMSR(uint32_t msr, uint32_t lo, uint32_t hi) {
  __asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

void writeMSR64(uint32_t msr, uint64_t value) {
  writeMSR(msr, value & 0xFFFFFFFF, (value >> 32) & 0xFFFFFFFF);
}