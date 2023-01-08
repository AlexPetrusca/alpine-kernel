#ifndef _CPUID_H_
#define _CPUID_H_

#include <kernel/cpu/msr.h>

//0	PE	Protected Mode Enable	If 1, system is in protected mode, else, system is in real mode
#define CR0_PE 1 << 0
//1	MP	Monitor co-processor	Controls interaction of WAIT/FWAIT instructions with TS flag in CR0
#define CR0_MP 1 << 1
//2	EM	Emulation	If set, no x87 floating-point unit present, if clear, x87 FPU present
#define CR0_EM 1 << 2
//3	TS	Task switched	Allows saving x87 task context upon a task switch only after x87 instruction used
#define CR0_TS 1 << 3
//4	ET	Extension type	On the 386, it allowed to specify whether the external math coprocessor was an 80287 or 80387
#define CR0_ET 1 << 4
//5	NE	Numeric error	Enable internal x87 floating point error reporting when set, else enables PC style x87 error detection
#define CR0_NE 1 << 5
//16	WP	Write protect	When set, the CPU can't write to read-only pages when privilege level is 0
#define CR0_WP 1 << 16
//18	AM	Alignment mask	Alignment check enabled if AM set, AC flag (in EFLAGS register) set, and privilege level is 3
#define CR0_AM 1 << 18
//29	NW	Not-write through	Globally enables/disable write-through caching
#define CR0_NW 1 << 29
//30	CD	Cache disable	Globally enables/disable the memory cache
#define CR0_CD 1 << 30
//31	PG	Paging	If 1, enable paging and use the § CR3 register, else disable paging.
#define CR0_PG 1 << 31

//0	VME	Virtual 8086 Mode Extensions	If set, enables support for the virtual interrupt flag (VIF) in virtual-8086 mode.
#define CR4_VME 1 << 0
//1	PVI	Protected-mode Virtual Interrupts	If set, enables support for the virtual interrupt flag (VIF) in protected mode.
#define CR4_PVI 1 << 1
//2	TSD	Time Stamp Disable	If set, RDTSC instruction can only be executed when in ring 0, otherwise RDTSC can be used at any privilege level.
#define CR4_TDS 1 << 2
//3	DE	Debugging Extensions	If set, enables debug register based breaks on I/O space access.
#define CR4_DE 1 << 3
//4	PSE	Page Size Extension	If set, enables 32-bit paging mode to use 4 MiB huge pages in addition to 4 KiB pages. If PAE is enabled or the processor is in x86-64 long mode this bit is ignored.[2]
#define CR4_PSE 1 << 4
//5	PAE	Physical Address Extension	If set, changes page table layout to translate 32-bit virtual addresses into extended 36-bit physical addresses.
#define CR4_PAE 1 << 5
//6	MCE	Machine Check Exception	If set, enables machine check interrupts to occur.
#define CR4_MCE 1 << 6
//7	PGE	Page Global Enabled	If set, address translations (PDE or PTE records) may be shared between address spaces.
#define CR4_PGE 1 << 7
//8	PCE	Performance-Monitoring Counter enable	If set, RDPMC can be executed at any privilege level, else RDPMC can only be used in ring 0.
#define CR4_PCE 1 << 8
//9	OSFXSR	Operating system support for FXSAVE and FXRSTOR instructions	If set, enables Streaming SIMD Extensions (SSE) instructions and fast FPU save & restore.
#define CR4_OSFXSR 1 << 9
//10	OSXMMEXCPT	Operating System Support for Unmasked SIMD Floating-Point Exceptions	If set, enables unmasked SSE exceptions.
#define CR4_OSXMMEXCPT 1 << 10
//11	UMIP	User-Mode Instruction Prevention	If set, the SGDT, SIDT, SLDT, SMSW and STR instructions cannot be executed if CPL > 0.[1]
#define CR4_UMIP 1 << 11
//12	LA57	57-Bit Linear Addresses	If set, enables 5-Level Paging.[3][4]: 2–18 
#define CR4_LA57 1 << 12
//13	VMXE	Virtual Machine Extensions Enable	see Intel VT-x x86 virtualization.
#define CR4_VMXE 1 << 13
//14	SMXE	Safer Mode Extensions Enable	see Trusted Execution Technology (TXT)
#define CR4_SMXE 1 << 14
//16	FSGSBASE	Enables the instructions RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE.
#define CR4_FSGSBASE 1 << 16
//17	PCIDE	PCID Enable	If set, enables process-context identifiers (PCIDs).
#define CR4_PCIDE 1 << 17
//18	OSXSAVE	XSAVE and Processor Extended States Enable
#define CR4_OSXSAVE 1 << 18
//20	SMEP[5]	Supervisor Mode Execution Protection Enable	If set, execution of code in a higher ring generates a fault.
#define CR4_SMEP 1 << 20
//21	SMAP	Supervisor Mode Access Prevention Enable	If set, access of data in a higher ring generates a fault.[6]
#define CR4_SMAP 1 << 21
//22	PKE	Protection Key Enable	See Intel 64 and IA-32 Architectures Software Developer’s Manual.
#define CR4_PKE 1 << 22
//23	CET	Control-flow Enforcement Technology	If set, enables control-flow enforcement technology.[4]: 2–19 
#define CR4_CET 1 << 23
//24	PKS	Enable Protection Keys for Supervisor-Mode Pages	If set, each supervisor-mode linear address is associated with a protection key when 4-level or 5-level paging is in use.[4]: 2–19 
#define CR4_PKS 1 << 24

// Features obtained in EDX by calling CPUID 1

// Page Size Extension
# define CPU_FEATURE_PSE 1 << 3
// Model-specific registers
# define CPU_FEATURE_MSR 1 << 5
// Physical Address Extension
# define CPU_FEATURE_PAE 1 << 6
// Onboard Advanced Programmable Interrupt Controller
# define CPU_FEATURE_APIC 1 << 9
// Page Global Enable bit in CR4
# define CPU_FEATURE_PGE 1 << 13
// 36-bit page size extension
# define CPU_FEATURE_PSE36 1 << 17
// Hyper Threading
# define CPU_FEATURE_HTT 1 << 28

void cpu_delay_us(long us);
void cpu_delay_ms(long ms);
void cpu_delay_s(long s);
void cpu_print_info(int argc, char** argv);

#endif //_CPUID_H_
