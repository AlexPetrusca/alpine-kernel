#ifndef _SYS_IO_H
#define _SYS_IO_H

#include <stdint.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
  asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
  uint16_t ret;
  asm volatile ( "inw %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
  asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
  uint32_t ret;
  asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

static inline void outl(uint16_t port, uint32_t val) {
  asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint64_t inq(uint16_t port) {
  uint64_t ret;
  asm volatile ( "inq %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

static inline void outq(uint16_t port, uint64_t val) {
  asm volatile ( "outq %0, %1" : : "a"(val), "Nd"(port));
}

static inline void io_wait(void) {
  outb(0x80, 0);
}

__END_DECLS

#endif
