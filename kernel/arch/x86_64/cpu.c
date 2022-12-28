#include <kernel/cpu.h>
#include <cpuid.h>
#include <stdio.h>

// Intel syntax: op dst, src (Intel manuals!)
// AT&T (gcc/gas) syntax: op src, dst (labs, xv6), uses b, w, l, q suffix on instructions to specify size of operands
// %q1 - any 64 bit register
// %k1 - any 32 bit register
// %h1 - any 8 bit register

/* x86-64 uses %rbx as the base register, so preserve it. */
/* output register a in __eax, etc. */
#define __cpuid64(__leaf, __eax, __ebx, __ecx, __edx)        \
  __asm(                                                   \
      "  xchgq %%rbx,%q1\n"                                \
      "  cpuid\n"                                          \
      "  xchgq  %%rbx,%q1"                                 \
      : "=a"(__eax), "=r"(__ebx), "=c"(__ecx), "=d"(__edx) \
      : "0"(__leaf))

#ifdef __x86_64__
#define __cr(_cr0, _cr2, _cr3, _cr4)                    \
  __asm(                                        \
      "  mov  %%cr0,%%rax\n"                    \
      "  mov  %%eax,%0\n"                        \
      "  mov  %%cr2,%%rax\n"                    \
      "  mov  %%eax,%1\n"                        \
      "  mov  %%cr3,%%rax\n"                    \
      "  mov  %%eax,%2\n"                        \
      "  mov  %%cr4,%%rax\n"                    \
      "  mov  %%eax,%3\n"                        \
      : "=rm"(_cr0), "=rm"(_cr2), "=rm"(_cr3), "=rm"(_cr4)    \
      : /* no input */                            \
      : "%rax"                                    \
      )
#else
#define __cr(_cr0, _cr2, _cr3, _cr4)                    \
  __asm(                                        \
      "  mov  %%cr0,%0\n"                        \
      "  mov  %%cr2,%1\n"                        \
      "  mov  %%cr3,%2\n"                        \
      "  mov  %%cr4,%3\n"                        \
      : "=rm"(_cr0), "=rm"(_cr2), "=rm"(_cr3), "=rm"(_cr4)    \
      : /* no input */                            \
      : "%rax"                                    \
      )
#endif

void print_cpu_info() {
  uint32_t id[13];
  id[12] = 0;  // to create a zero-terminated string
  __cpuid(0x80000002, id[0], id[1], id[2], id[3]);
  __cpuid(0x80000003, id[4], id[5], id[6], id[7]);
  __cpuid(0x80000004, id[8], id[9], id[10], id[11]);
  printf("CPU Brand: %s\n", (char*) id);

  uint32_t eax, ebx, ecx, edx;
  uint32_t mid[4];
  mid[3] = 0;  // to create a zero-terminated string
  __cpuid(0, eax, mid[0], mid[2], mid[1]);
  printf("Manufacturer ID: %s\n", (char*) mid);
  printf("Highest basic calling parameter: %d\n", eax);

  __cpuid(1, eax, ebx, ecx, edx);
  printf("Processor Version Info: %b\n", eax);
  printf("Hyper Threading Enabled: %d, Core Count: %d\n", (edx & CPU_FEATURE_HTT) != 0, (ebx >> 16) & 0xFF);
  printf("Onboard APIC Feature: %d, Local APIC ID: %d\n", (edx & CPU_FEATURE_APIC) != 0, (ebx >> 24) & 0xFF);
  printf("Model-specific registers Feature: %d\n", (edx & CPU_FEATURE_MSR) != 0);
  printf("Page Size Extension Feature: %d\n", (edx & CPU_FEATURE_PSE) != 0);
  printf("Physical Address Extension Feature: %d\n", (edx & CPU_FEATURE_PAE) != 0);
  printf("Page Global Enable bit in CR4 Feature: %d\n", (edx & CPU_FEATURE_PGE) != 0);
  printf("36-bit page size extension Feature: %d\n", (edx & CPU_FEATURE_PSE36) != 0);

  __cpuid(0x80000006, eax, ebx, ecx, edx);
  printf("L2 Cache Line size: %dB, Associativity Type: %d; Cache Size: %dKB\n",
      ecx & 0xff, (ecx >> 12) & 0x07, (ecx >> 16) & 0xffff);

  __cpuid(0x80000008, eax, ebx, ecx, edx);
  printf("Physical Address Bits: %d, Linear Address Bits: %d, Physical Cores: %d\n", eax & 0xff,
      (eax >> 8) & 0xff, ecx + 1);

  uint32_t cr0, cr2, cr3, cr4;
  __cr(cr0, cr2, cr3, cr4);
  printf("Protected Mode Enable: %d\n", (cr0 & CR0_PE) != 0);
  printf("Paging Enable: %d\n", (cr0 & CR0_PG) != 0);
  printf("Virtual 8086 Mode Extensions: %d\n", (cr4 & CR4_VME) != 0);
  printf("Protected-mode Virtual Interrupts: %d\n", (cr4 & CR4_PVI) != 0);
  printf("Page Size Extension: %d\n", (cr4 & CR4_PSE) != 0);
  printf("Physical Address Extension: %d\n", (cr4 & CR4_PAE) != 0);
  printf("Page Global Enabled: %d\n", (cr4 & CR4_PGE) != 0);

  uint64_t efer = readMSR64(IA32_EFER_MSR);
  printf("Long Mode Enable: %d\n", (efer & IA32_EFER_MSR_LME) != 0);
  printf("Long Mode Active: %d\n", (efer & IA32_EFER_MSR_LMA) != 0);
  printf("Long Mode Segment Limit Enable: %d\n", (efer & IA32_EFER_MSR_LMSLE) != 0);
  printf("System Call Extensions: %d\n", (efer & IA32_EFER_MSR_SCE) != 0);
}
