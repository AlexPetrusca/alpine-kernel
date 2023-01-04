#ifndef KERNEL_ARCH_I386_PCI_H_
#define KERNEL_ARCH_I386_PCI_H_

#include <kernel/acpi.h>
#include <stdint.h>
#include <kernel/error.h>

#define PCI_MAX_DEVICE 31
#define PCI_MAX_FUNCTION 7

#define PCI_HEADER_TYPE_DEVICE            0x00
#define PCI_HEADER_TYPE_PCI_TO_PCI_BRIDGE 0x01
#define PCI_HEADER_TYPE_CARDBUS_BRIDGE    0x02
#define PCI_HEADER_TYPE_MULTI_FUNCTION    0x80

typedef struct {
  uint64_t base_address;
  uint16_t segment_group_number;
  uint8_t start_bus_number;
  uint8_t end_bus_number;
  uint32_t Reserved;
} __attribute__((packed)) pci_host_bridge;

typedef struct {
  acpi_header header;
  uint64_t reserved;
  pci_host_bridge host_bridge[0];
} __attribute__((packed)) pci_mcfg_table;

typedef struct {
  uint16_t vendor_id;
  uint16_t device_id;
  uint16_t command;
  uint16_t status;
  uint8_t revision_id;
  uint8_t pi_class_code;
  uint8_t sub_class_code;
  uint8_t base_class_code;
  uint8_t cache_line_size;
  uint8_t master_latency_timer;
  uint8_t header_type;
  uint8_t bist;
} __attribute__((packed)) pci_header;

#define BAR_TYPE_MASK   0x00000001
#define BAR_TYPE_MEMORY 0
#define BAR_TYPE_IO     1

#define BAR_MEMORY_ADDRESS_MASK     0xFFFFFFF0
#define BAR_IO_ADDRESS_MASK         0xFFFFFFF3

#define BAR_LOCATABLE_MASK    0
#define BAR_LOCATABLE_32BIT   0
#define BAR_LOCATABLE_BELOW1M 1
#define BAR_LOCATABLE_64BIT   2

typedef union {
  uint32_t value;
  struct {
    uint32_t type: 1;  //  BAR_TYPE_MEMORY or BAR_TYPE_IO
    union {
      struct {
        uint32_t locatable: 2;  // 0 = any 32-bit, 1 = < 1 MB, 2 = any 64-bit
        uint32_t prefetchable: 1; // 0 = no, 1 = yes
        uint32_t base_address: 28;
      } __attribute__((packed)) memory;
      struct {
        uint32_t reserved: 1;
        uint32_t base_address: 32;
      } __attribute__((packed)) io;
    }__attribute__((packed));
  }__attribute__((packed));
} __attribute__((packed)) pci_bar;

//static_assert(sizeof(pci_bar) == 4, "sizeof(pci_bar) must be 4");

typedef struct {
  pci_header header;
  pci_bar bar[6];
  // incomplete
} __attribute__((packed)) pci_type0_config;

typedef struct {
  pci_header header;
  pci_bar bar[2];
  // incomplete
} __attribute__((packed)) pci_type1_config;

typedef struct {
  char* base_class; // Pointer to the PCI base class string
  char* sub_class;  // Pointer to the PCI sub class string
  char* pif_class;  // Pointer to the PCI programming interface string
} pci_class_cames;

typedef struct {
  uint8_t bus_number;
  uint8_t device_number;
  uint8_t function_number;
  uint16_t vendor_id;
  uint16_t device_id;
  uint8_t header_type;
  uint8_t pi_class_code;
  uint8_t sub_class_code;
  uint8_t base_class_code;
  uint32_t ecam_adress;
  pci_bar bar[6];
} pci_device;

bool pci_init() __attribute__ ((warn_unused_result));
bool pci_get_device(pci_device* device) __attribute__ ((warn_unused_result));
void pci_get_class_names(pci_device* device, pci_class_cames* names);
uint64_t pci_bar_addr_32(pci_device* device, int bar_index);
uint64_t pci_bar_addr_64(pci_device* device, int bar_index);

void pci_print_mcfg(int argc, char** argv);
void pci_print_devices(int argc, char** argv);

#endif //KERNEL_ARCH_I386_PCI_H_
