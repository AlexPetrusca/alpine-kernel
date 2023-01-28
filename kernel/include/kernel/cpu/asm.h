#ifndef KERNEL_INCLUDE_KERNEL_CPU_ASM_H_
#define KERNEL_INCLUDE_KERNEL_CPU_ASM_H_

#include <stdint.h>

// Intel syntax: op dst, src (Intel manuals!)
// AT&T (gcc/gas) syntax: op src, dst (labs, xv6), uses b, w, l, q suffix on instructions to specify size of operands
// %q1 - any 64 bit register
// %k1 - any 32 bit register
// %h1 - any 8 bit register

#define LIDT(idt_desc)    __asm volatile ("lidt %0" : : "m"(idt_desc))
#define CLI()             __asm volatile ("cli")
#define STI()             __asm volatile ("sti")
#define INT(n)            __asm volatile ("int %0" : : "i"(n))

#define PUSH(n)           __asm volatile ("push %0" : : "i"(n))
#define POP_RAX()         __asm volatile ("pop %%rax" : : )

#define RDMSR(msr, lo, hi)  __asm volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr))
#define WRMSR(msr, lo, hi)  __asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr))

/* x86-64 uses %rbx as the base register, so preserve it. */
/* output register a in __eax, etc. */
#define __cpuid64(__leaf, __eax, __ebx, __ecx, __edx)      \
  __asm(                                                  \
    "  xchgq %%rbx,%q1\n"                                 \
    "  cpuid\n"                                           \
    "  xchgq  %%rbx,%q1"                                  \
    : "=a"(__eax), "=r"(__ebx), "=c"(__ecx), "=d"(__edx)  \
    : "0"(__leaf))

#define __cr(_cr0, _cr2, _cr3, _cr4)                       \
  __asm(                                                  \
    "  mov  %%cr0,%%rax\n"                                \
    "  mov  %%rax,%0\n"                                   \
    "  mov  %%cr2,%%rax\n"                                \
    "  mov  %%rax,%1\n"                                   \
    "  mov  %%cr3,%%rax\n"                                \
    "  mov  %%rax,%2\n"                                   \
    "  mov  %%cr4,%%rax\n"                                \
    "  mov  %%rax,%3\n"                                   \
    : "=rm"(_cr0), "=rm"(_cr2), "=rm"(_cr3), "=rm"(_cr4)  \
    : /* no input */                                      \
    : "%rax"                                              \
  )

#define __cr2(_cr2)                                       \
  __asm(                                                  \
    "  mov  %%cr2,%0\n"                                   \
    : "=rm"(_cr2)                                         \
    : /* no input */                                      \
    : /* no affected regs */                              \
  )

static inline uint64_t ROR(uint64_t value, int i) {
  __asm__("rorq %%cl,%0" :"=r" (value) :"0" (value), "c" (i));
  return value;
}

static inline uint64_t ROL(uint64_t value, int i) {
  __asm__("rolq %%cl,%0" :"=r" (value) :"0" (value), "c" (i));
  return value;
}

static inline uint64_t rdtsc() {
  unsigned int lo, hi;
  __asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
  return ((uint64_t) hi << 32) | lo;
}

#endif //KERNEL_INCLUDE_KERNEL_CPU_ASM_H_
