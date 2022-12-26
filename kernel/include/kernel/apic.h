#ifndef _APIC_H_
#define _APIC_H_

#include <stdint.h>

typedef enum {
  APIC_REG_ID = 0x020, //ID Register   (RW)
  APIC_REG_VR = 0x030, //Version Register (R)
  APIC_REG_TPR = 0x080, //Task Priority Register (TPR) (RW)
  APIC_REG_APR = 0x090, //Arbitration Priority Register (APR) (R)
  APIC_REG_PPR = 0x0A0, //Processor Priority Register (PPR) (R)
  APIC_REG_EOI = 0x0B0, //EOI register (W)
  APIC_REG_RRD = 0x0C0, //Remote Read Register (RRD) (R)
  APIC_REG_LDR = 0x0D0, //Logical Destination Register (RW)
  APIC_REG_DFR = 0x0E0, //Destination Format Register (RW)
  APIC_REG_SIV = 0x0f0, //Spurious Interrupt Vector Register (RW)
  //APIC_REG_ISR=0x100 - 0x170 In-Service Register (ISR) (R)
  //APIC_REG_TMR=0x180 - 0x1F0 Trigger Mode Register (TMR) (R)
  //APIC_REG_IRR=0x200 - 0x270 Interrupt Request Register (IRR) (R)
  APIC_REG_ESR = 0x280, //Error Status Register (R)
  APIC_REG_CCMIR = 0x2F0, //LVT Corrected Machine Check Interrupt (CMCI) Register (RW)
  APIC_REG_ICR_LOW = 0x300,   //Interrupt Command Register (ICR) (RW)
  APIC_REG_ICR_HIGH = 0x310,  //Interrupt Command Register (ICR) (RW)
  APIC_REG_TR = 0x320, //LVT Timer Register (RW)
  APIC_REG_TSR = 0x330, //LVT Thermal Sensor Register (RW)
  APIC_REG_PMCR = 0x340, //LVT Performance Monitoring Counters Register (RW)
  APIC_REG_LINT0R = 0x350, //LVT LINT0 Register (RW)
  APIC_REG_LINT1R = 0x360, //LVT LINT1 Register (RW)
  APIC_REG_ER = 0x370, //LVT Error Register (RW)
  APIC_REG_ICR = 0x380, //Initial Count Register (for Timer) (RW)
  APIC_REG_CCR = 0x390, //Current Count Register (for Timer) (R)
  APIC_REG_DCR = 0x3E0, //Divide Configuration Register (for Timer) (RW)
} ApicReg;

#define APIC_REG_SIV_ENABLE (1 << 8)

uint32_t globalEnableLAPIC();
void enableLAPIC(uint64_t apic_base_addr);
void sendLapicInit(uint64_t apic_base_addr);

uint32_t readApicReg(ApicReg reg, uint64_t apic_base_addr);
void writeApicReg(ApicReg reg, uint32_t value, uint64_t apic_base_addr);

void printLapicInfo();

#endif //_APIC_H_
