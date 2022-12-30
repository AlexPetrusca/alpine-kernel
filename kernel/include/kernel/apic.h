#ifndef _APIC_H_
#define _APIC_H_

#include <stdint.h>
#include <kernel/acpi.h>

#define LOCAL_APIC 0
#define IO_APIC 1
#define IO_APIC_INTERRUPTS_SOURCE_OVERRIDE 2
#define LOCAL_APIC_NON_MASKABLE_INTERRUPTS 4

/// Multiple APIC Description Table header definition.  The rest of the table
/// must be defined in a platform specific manner.
typedef struct {
  acpi_header header;
  uint32_t local_apic_address;
  uint32_t flags;
} __attribute__((packed)) apic_table_header;

typedef struct {
  uint8_t entry_type;
  uint8_t record_length;
} __attribute__((packed)) apic_entry_header;

typedef struct {
  apic_entry_header header;
  uint8_t acpi_processor_id;
  uint8_t apic_id;
  uint32_t flags; // (bit 0 = Processor Enabled) (bit 1 = Online Capable)
} __attribute__((packed)) apic_lapic_entry;

typedef struct {
  apic_entry_header header;
  uint8_t apic_id;
  uint8_t reserved;
  uint32_t ioapic_address;
  uint32_t global_system_interrupt_base;
} __attribute__((packed)) apic_ioapic_entry;

typedef struct {
  apic_entry_header header;
  uint8_t bus_source;
  uint8_t irq_source;
  uint32_t global_system_interrupt;
  uint16_t flags;
} __attribute__((packed)) apic_ioapic_isr_entry; // ISR = Interrupt Source Override

typedef struct {
  apic_entry_header header;
  uint8_t acpi_processor_id; // 0xFF means all processors
  uint8_t flags;
  uint8_t lint; // 0 or 1
} __attribute__((packed)) apic_lapic_nmi_entry; // NMI = NON_MASKABLE_INTERRUPTS

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
} apic_reg;

#define APIC_REG_SIV_ENABLE (1 << 8)

uint32_t apic_global_enable();
void apic_enable(uint64_t apic_base_addr);
void apic_send_init(uint64_t apic_base_addr);
uint32_t apic_read_reg(apic_reg reg, uint64_t apic_base_addr);
void apic_write_reg(apic_reg reg, uint32_t value, uint64_t apic_base_addr);
void apic_print_info();
void apic_print_lapic_info();

#endif //_APIC_H_
