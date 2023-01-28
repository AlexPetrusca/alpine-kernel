#ifndef ALPINE_KERNEL_ISR_H
#define ALPINE_KERNEL_ISR_H

#include <stdint.h>

#include <sys/circ_buf.h>
#include <kernel/cpu/pic.h>

#define Division_Error_VECTOR                           0x00
#define Debug                                           0x01
#define Non_Maskable_Interrupt                          0x02
#define Breakpoint                                      0x03
#define Overflow                                         0x04
#define Bound_Range_Exceeded                            0x05
#define Invalid_Opcode                                  0x06
#define Device_Not_Available                            0x07
#define Double_Fault                                    0x08
#define Coprocessor_Segment_Overrun                     0x09
#define Invalid_TSS                                     0x0A
#define Segment_Not_Present                             0x0B
#define Stack_Segment_Fault                             0x0C
#define General_Protection_Fault                        0x0D
#define PAGE_FAULT_VECTOR                               0x0E
#define x87_Floating_Point_Exception                    0x10
#define Alignment_Check                                 0x11
#define Machine_Check                                   0x12
#define SIMD_Floating_Point_Exception                   0x13
#define Virtualization_Exception                        0x14
#define Control_Protection_Exception                    0x15
#define Hypervisor_Injection_Exception                  0x1C
#define VMM_Communication_Exception                     0x1D
#define Security_Exception                              0x1E

#define PIT_VECTOR                                      (PIC1_START_VECTOR + PIC_PIT_IRQ)
#define KEYBOARD_VECTOR                                 (PIC1_START_VECTOR + PIC_KEYBOARD_IRQ)

typedef struct {
  uint64_t CF: 1;
  uint64_t   : 1;
  uint64_t PF: 1;
  uint64_t   : 1;
  uint64_t AF: 1;
  uint64_t   : 1;
  uint64_t ZF: 1;
  uint64_t SF: 1;
  uint64_t TF: 1;
  uint64_t IF: 1;
  uint64_t DF: 1;
  uint64_t OF: 1;
  uint64_t IOPL: 2;
  uint64_t NT: 1;
  uint64_t MD: 1;
  uint64_t RF: 1;
  uint64_t VM: 1;
  uint64_t AC: 1;
  uint64_t VIF: 1;
  uint64_t VIP: 1;
  uint64_t ID: 1;
  uint64_t   : 42;
} __attribute__((packed)) rflags;
static_assert(sizeof(rflags) == 8, "");

typedef struct {
  // the last register saved on the stack, lowest address
  uint64_t cr3;
  uint64_t cr0;
  uint64_t cr2;
  uint64_t cr4;
  uint64_t es;
  uint64_t ds;
  uint64_t gs;
  uint64_t fs;
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rbp;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rbx;
  uint64_t rax;
  uint8_t fx[512];              // all FPU/MMX/SSE registers
  uint64_t available;           // this is the 16-alignment value required by fxsave
  uint64_t vector_num;
  uint64_t error_code;          // error code is pushed first by the CPU, highest address
  uint64_t rip;
  uint64_t cs;
  rflags rfl;
  uint64_t rsp;
  uint64_t ss;
} __attribute__((packed)) interrupt_frame;
static_assert(sizeof(interrupt_frame) == 760, "");

uint64_t main_isr(interrupt_frame* frame);
void isr_print_info(int argc, char** argv);

#endif //ALPINE_KERNEL_ISR_H
