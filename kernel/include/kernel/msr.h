#ifndef _MSR_H_
#define _MSR_H_

#include <stdint.h>

#define IA32_EFER_MSR             0xc0000080
// System Call Extensions (SCE)
#define IA32_EFER_MSR_SCE         1 << 0
// Long Mode Enable (LME)
#define IA32_EFER_MSR_LME         1 << 8
// Long Mode Active (LMA)
#define IA32_EFER_MSR_LMA         1 << 10
// No-Execute Enable (NXE)
#define IA32_EFER_MSR_NXE         1 << 11
// Secure Virtual Machine Enable (SVME)
#define IA32_EFER_MSR_SVME        1 << 12
// Long Mode Segment Limit Enable (LMSLE)
#define IA32_EFER_MSR_LMSLE       1 << 13
// Fast FXSAVE/FXSTOR (FFXSR)
#define IA32_EFER_MSR_FFXSR       1 << 14
// Translation Cache Extension (TCE)
#define IA32_EFER_MSR_TCE         1 << 15

// bit 8: IS_BSP flag - Indicates if the processor is the bootstrap processor (BSP).
// bit 11: APIC Global Enable flag - Enables or disables the local APIC.
// bits 12 through 35: APIC Base field - Specifies the base address of the APIC registers. This 24-bit value is
// extended by 12 bits at the low end to form the base address. This automatically aligns the address on a
// 4-KByte boundary. Following a power-up or RESET the field is set to FEE0 0000H.
// All other bits are reserved.
#define IA32_APIC_BASE_MSR          0x1b
#define IA32_APIC_BASE_MSR_BSP      (1 << 8)
#define IA32_APIC_BASE_MSR_ENABLE   (1 << 11)

#define IS_BSP(apicBaseMSR)                        ((apicBaseMSR & IA32_APIC_BASE_MSR_BSP) != 0)
#define IS_APIC_GLOBAL_ENABLE(apicBaseMSR)         (apicBaseMSR & IA32_APIC_BASE_MSR_ENABLE)
#define SET_APIC_GLOBAL_ENABLE(apicBaseMSR)        (apicBaseMSR |= IA32_APIC_BASE_MSR_ENABLE)
#define GET_APIC_BASE(apicBaseMSR)                 ((apicBaseMSR >> 12) << 12)

#define GET_APIC_BASE(apicBaseMSR)                 ((apicBaseMSR >> 12) << 12)

void readMSR(uint32_t msr, uint32_t* lo, uint32_t* hi);
uint64_t readMSR64(uint32_t msr);

void writeMSR(uint32_t msr, uint32_t lo, uint32_t hi);
void writeMSR64(uint32_t msr, uint64_t value);

#endif //_MSR_H_