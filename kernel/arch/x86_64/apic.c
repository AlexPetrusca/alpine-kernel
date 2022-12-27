#include <kernel/apic.h>
#include <kernel/cpu.h>
#include <stdio.h>

uint32_t readApicReg(ApicReg reg, uint64_t apic_base_addr) {
  return *((uint32_t*) (apic_base_addr + reg));
}

void writeApicReg(ApicReg reg, uint32_t value, uint64_t apic_base_addr) {
  *((uint32_t*) (apic_base_addr + reg)) = value;
}

uint32_t globalEnableLAPIC() {
  uint64_t apic_base_msr = readMSR64(IA32_APIC_BASE_MSR);
  if (!IS_APIC_GLOBAL_ENABLE(apic_base_msr)) {
    SET_APIC_GLOBAL_ENABLE(apic_base_msr);
    writeMSR64(IA32_APIC_BASE_MSR, apic_base_msr);
  }
  return GET_APIC_BASE(apic_base_msr);
}

void enableLAPIC(uint64_t apic_base_addr) {
  uint32_t siv = readApicReg(APIC_REG_SIV, apic_base_addr);
  if (!(siv & APIC_REG_SIV_ENABLE)) {
    writeApicReg(APIC_REG_SIV, siv | APIC_REG_SIV_ENABLE, apic_base_addr);
  }
}

void sendLapicInit(uint64_t apic_base_addr) {
  writeApicReg(APIC_REG_ICR_HIGH, 0, apic_base_addr);
  //  Vector: 00h
  //  Delivery mode: Startup
  //  Destination mode: ignored (0)
  //  Level: ignored (1)
  //  Trigger mode: ignored (0)
  //  Shorthand: All excluding self (3)
  //  Writing the low DWORD sent the IPI
  writeApicReg(APIC_REG_ICR_LOW, 0x0c4500, apic_base_addr);
}

void printLapicInfo() {
  uint32_t apic_base_addr = globalEnableLAPIC();
  printf("LAPIC ID: %d\n", readApicReg(APIC_REG_ID, apic_base_addr));
  printf("LAPIC Version: %d\n", readApicReg(APIC_REG_VR, apic_base_addr));
  printf("LAPIC PPR: %d\n", readApicReg(APIC_REG_PPR, apic_base_addr));
}