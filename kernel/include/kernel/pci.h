#ifndef KERNEL_ARCH_I386_PCI_H_
#define KERNEL_ARCH_I386_PCI_H_

#include <kernel/acpi.h>
#include <stdint.h>

#define PCI_MAX_DEVICE 31
#define PCI_MAX_FUNCTION 7

#define HEADER_TYPE_DEVICE            0x00
#define HEADER_TYPE_PCI_TO_PCI_BRIDGE 0x01
#define HEADER_TYPE_CARDBUS_BRIDGE    0x02
#define HEADER_TYPE_MULTI_FUNCTION    0x80

typedef struct {
  uint64_t BaseAddress;
  uint16_t SegmentGroupNumber;
  uint8_t StartBusNumber;
  uint8_t EndBusNumber;
  uint32_t Reserved;
} PciHostBridge;

typedef struct {
  AcpiDescriptionHeader Header;
  uint64_t Reserved;
  PciHostBridge HostBridge[0];
} McfgTable;

typedef struct {
  uint16_t VendorId;
  uint16_t DeviceId;
  uint16_t Command;
  uint16_t Status;
  uint8_t RevisionId;
  uint8_t PIClassCode;
  uint8_t SubClassCode;
  uint8_t BaseClassCode;
  uint8_t CacheLineSize;
  uint8_t MasterLatencyTimer;
  uint8_t HeaderType;
  uint8_t BIST;
} __attribute__((packed)) PciHeader;

typedef struct {
  char* BaseClass; // Pointer to the PCI base class string
  char* SubClass;  // Pointer to the PCI sub class string
  char* PIFClass;  // Pointer to the PCI programming interface string
} PciClassNames;

typedef struct {
  uint8_t BusNumber;
  uint8_t DeviceNumber;
  uint8_t FunctionNumber;
  uint16_t VendorId;
  uint16_t DeviceId;
  uint8_t HeaderType;
  uint8_t PIClassCode;
  uint8_t SubClassCode;
  uint8_t BaseClassCode;
  uint32_t EcamAdress;
} PciDevice;

void pci_enumerate();
void pci_GetClassStrings(PciDevice* device, PciClassNames* names);
void pci_PrintMcfg();
void pci_PrintDevices();

#endif //KERNEL_ARCH_I386_PCI_H_
