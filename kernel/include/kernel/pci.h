#ifndef KERNEL_ARCH_I386_PCI_H_
#define KERNEL_ARCH_I386_PCI_H_

#include <kernel/acpi.h>
#include <stdint.h>

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
} pci_device;

void pci_init();
void pci_get_class_names(pci_device* device, pci_class_cames* names);
void pci_print_mcfg();
void pci_print_devices();

#endif //KERNEL_ARCH_I386_PCI_H_
