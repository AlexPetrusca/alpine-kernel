#include <kernel/pci.h>
#include <stdio.h>
#include <kernel/mem.h>

#define MAX_PCI_DEVICES 32

pci_device _pci_devices[MAX_PCI_DEVICES] = {0};
int _pci_device_count = 0;

pci_mcfg_table* find_mcfg_table() {
  return (pci_mcfg_table*) acpi_find_table("MCFG");
}

typedef struct _PciClassEntry {
  uint32_t Code;                          // Class, subclass or I/F code
  char* Description;                      // Description string
  struct _PciClassEntry* LowerLevelClass; // Subclass or I/F if any
} PciClassEntry;

// Declarations of entries which contain printable strings for class codes in PCI configuration space
PciClassEntry PCIBlankEntry[];
PciClassEntry PCISubClass_00[];
PciClassEntry PCISubClass_01[];
PciClassEntry PCISubClass_02[];
PciClassEntry PCISubClass_03[];
PciClassEntry PCISubClass_04[];
PciClassEntry PCISubClass_05[];
PciClassEntry PCISubClass_06[];
PciClassEntry PCISubClass_07[];
PciClassEntry PCISubClass_08[];
PciClassEntry PCISubClass_09[];
PciClassEntry PCISubClass_0a[];
PciClassEntry PCISubClass_0b[];
PciClassEntry PCISubClass_0c[];
PciClassEntry PCISubClass_0d[];
PciClassEntry PCISubClass_0e[];
PciClassEntry PCISubClass_0f[];
PciClassEntry PCISubClass_10[];
PciClassEntry PCISubClass_11[];
PciClassEntry PCISubClass_12[];
PciClassEntry PCISubClass_13[];
PciClassEntry PCIPIFClass_0100[];
PciClassEntry PCIPIFClass_0101[];
PciClassEntry PCIPIFClass_0105[];
PciClassEntry PCIPIFClass_0106[];
PciClassEntry PCIPIFClass_0107[];
PciClassEntry PCIPIFClass_0108[];
PciClassEntry PCIPIFClass_0109[];
PciClassEntry PCIPIFClass_0300[];
PciClassEntry PCIPIFClass_0604[];
PciClassEntry PCIPIFClass_0609[];
PciClassEntry PCIPIFClass_060b[];
PciClassEntry PCIPIFClass_0700[];
PciClassEntry PCIPIFClass_0701[];
PciClassEntry PCIPIFClass_0703[];
PciClassEntry PCIPIFClass_0800[];
PciClassEntry PCIPIFClass_0801[];
PciClassEntry PCIPIFClass_0802[];
PciClassEntry PCIPIFClass_0803[];
PciClassEntry PCIPIFClass_0904[];
PciClassEntry PCIPIFClass_0c00[];
PciClassEntry PCIPIFClass_0c03[];
PciClassEntry PCIPIFClass_0c07[];
PciClassEntry PCIPIFClass_0d01[];
PciClassEntry PCIPIFClass_0e00[];

//
// Base class strings entries
//
PciClassEntry gClassStringList[] = {
  {0x00, "Pre 2.0 device", PCISubClass_00},
  {0x01, "Mass Storage Controller", PCISubClass_01},
  {0x02, "Network Controller", PCISubClass_02},
  {0x03, "Display Controller", PCISubClass_03},
  {0x04, "Multimedia Device", PCISubClass_04},
  {0x05, "Memory Controller", PCISubClass_05},
  {0x06, "Bridge Device", PCISubClass_06},
  {0x07, "Simple Communications Controllers", PCISubClass_07},
  {0x08, "Base System Peripherals", PCISubClass_08},
  {0x09, "Input Devices", PCISubClass_09},
  {0x0a, "Docking Stations", PCISubClass_0a},
  {0x0b, "Processors", PCISubClass_0b},
  {0x0c, "Serial Bus Controllers", PCISubClass_0c},
  {0x0d, "Wireless Controllers", PCISubClass_0d},
  {0x0e, "Intelligent IO Controllers", PCISubClass_0e},
  {0x0f, "Satellite Communications Controllers", PCISubClass_0f},
  {0x10, "Encryption/Decryption Controllers", PCISubClass_10},
  {0x11, "Data Acquisition & Signal Processing Controllers", PCISubClass_11},
  {0x12, "Processing Accelerators", PCISubClass_12},
  {0x13, "Non-Essential Instrumentation", PCISubClass_13},
  {0xff, "Device does not fit in any defined classes", PCIBlankEntry},
  {0x00, NULL, NULL}
};

//
// Subclass strings entries
//
PciClassEntry PCIBlankEntry[] = {
  {0x00, "", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_00[] = {
  {0x00, "All devices other than VGA", PCIBlankEntry},
  {0x01, "VGA-compatible devices", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_01[] = {
  {0x00, "SCSI", PCIPIFClass_0100},
  {0x01, "IDE controller", PCIPIFClass_0101},
  {0x02, "Floppy disk controller", PCIBlankEntry},
  {0x03, "IPI controller", PCIBlankEntry},
  {0x04, "RAID controller", PCIBlankEntry},
  {0x05, "ATA controller with ADMA interface", PCIPIFClass_0105},
  {0x06, "Serial ATA controller", PCIPIFClass_0106},
  {0x07, "Serial Attached SCSI (SAS) controller ", PCIPIFClass_0107},
  {0x08, "Non-volatile memory subsystem", PCIPIFClass_0108},
  {0x09, "Universal Flash Storage (UFS) controller ", PCIPIFClass_0109},
  {0x80, "Other mass storage controller", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_02[] = {
  {0x00, "Ethernet controller", PCIBlankEntry},
  {0x01, "Token ring controller", PCIBlankEntry},
  {0x02, "FDDI controller", PCIBlankEntry},
  {0x03, "ATM controller", PCIBlankEntry},
  {0x04, "ISDN controller", PCIBlankEntry},
  {0x05, "WorldFip controller", PCIBlankEntry},
  {0x06, "PICMG 2.14 Multi Computing", PCIBlankEntry},
  {0x07, "InfiniBand controller", PCIBlankEntry},
  {0x80, "Other network controller", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_03[] = {
  {0x00, "VGA/8514 controller", PCIPIFClass_0300},
  {0x01, "XGA controller", PCIBlankEntry},
  {0x02, "3D controller", PCIBlankEntry},
  {0x80, "Other display controller", PCIBlankEntry},
  {0x00, NULL, PCIBlankEntry}
};

PciClassEntry PCISubClass_04[] = {
  {0x00, "Video device", PCIBlankEntry},
  {0x01, "Audio device", PCIBlankEntry},
  {0x02, "Computer Telephony device", PCIBlankEntry},
  {0x03, "Mixed mode device", PCIBlankEntry},
  {0x80, "Other multimedia device", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_05[] = {
  {0x00, "RAM memory controller", PCIBlankEntry},
  {0x01, "Flash memory controller", PCIBlankEntry},
  {0x80, "Other memory controller", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_06[] = {
  {0x00, "Host/PCI bridge", PCIBlankEntry},
  {0x01, "PCI/ISA bridge", PCIBlankEntry},
  {0x02, "PCI/EISA bridge", PCIBlankEntry},
  {0x03, "PCI/Micro Channel bridge", PCIBlankEntry},
  {0x04, "PCI/PCI bridge", PCIPIFClass_0604},
  {0x05, "PCI/PCMCIA bridge", PCIBlankEntry},
  {0x06, "NuBus bridge", PCIBlankEntry},
  {0x07, "CardBus bridge", PCIBlankEntry},
  {0x08, "RACEway bridge", PCIBlankEntry},
  {0x09, "Semi-transparent PCI-to-PCI bridge", PCIPIFClass_0609},
  {0x0A, "InfiniBand-to-PCI host bridge", PCIBlankEntry},
  {0x0B, "Advanced Switching to PCI host bridge", PCIPIFClass_060b},
  {0x80, "Other bridge type", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_07[] = {
  {0x00, "Serial controller", PCIPIFClass_0700},
  {0x01, "Parallel port", PCIPIFClass_0701},
  {0x02, "Multiport serial controller", PCIBlankEntry},
  {0x03, "Modem", PCIPIFClass_0703},
  {0x04, "GPIB (IEEE 488.1/2) controller", PCIBlankEntry},
  {0x05, "Smart Card", PCIBlankEntry},
  {0x80, "Other communication device", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_08[] = {
  {0x00, "PIC", PCIPIFClass_0800},
  {0x01, "DMA controller", PCIPIFClass_0801},
  {0x02, "System timer", PCIPIFClass_0802},
  {0x03, "RTC controller", PCIPIFClass_0803},
  {0x04, "Generic PCI Hot-Plug controller", PCIBlankEntry},
  {0x05, "SD Host controller", PCIBlankEntry},
  {0x06, "IOMMU", PCIBlankEntry},
  {0x07, "Root Complex Event Collector", PCIBlankEntry},
  {0x80, "Other system peripheral", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_09[] = {
  {0x00, "Keyboard controller", PCIBlankEntry},
  {0x01, "Digitizer (pen)", PCIBlankEntry},
  {0x02, "Mouse controller", PCIBlankEntry},
  {0x03, "Scanner controller", PCIBlankEntry},
  {0x04, "Gameport controller", PCIPIFClass_0904},
  {0x80, "Other input controller", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_0a[] = {
  {0x00, "Generic docking station", PCIBlankEntry},
  {0x80, "Other type of docking station", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_0b[] = {
  {0x00, "386", PCIBlankEntry},
  {0x01, "486", PCIBlankEntry},
  {0x02, "Pentium", PCIBlankEntry},
  {0x10, "Alpha", PCIBlankEntry},
  {0x20, "PowerPC", PCIBlankEntry},
  {0x30, "MIPS", PCIBlankEntry},
  {0x40, "Co-processor", PCIBlankEntry},
  {0x80, "Other processor", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_0c[] = {
  {0x00, "IEEE 1394", PCIPIFClass_0c00},
  {0x01, "ACCESS.bus", PCIBlankEntry},
  {0x02, "SSA", PCIBlankEntry},
  {0x03, "USB", PCIPIFClass_0c03},
  {0x04, "Fibre Channel", PCIBlankEntry},
  {0x05, "System Management Bus", PCIBlankEntry},
  {0x06, "InfiniBand", PCIBlankEntry},
  {0x07, "IPMI", PCIPIFClass_0c07},
  {0x08, "SERCOS Interface Standard (IEC 61491)", PCIBlankEntry},
  {0x09, "CANbus", PCIBlankEntry},
  {0x80, "Other bus type", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_0d[] = {
  {0x00, "iRDA compatible controller", PCIBlankEntry},
  {0x01, "", PCIPIFClass_0d01},
  {0x10, "RF controller", PCIBlankEntry},
  {0x11, "Bluetooth", PCIBlankEntry},
  {0x12, "Broadband", PCIBlankEntry},
  {0x20, "Ethernet (802.11a - 5 GHz)", PCIBlankEntry},
  {0x21, "Ethernet (802.11b - 2.4 GHz)", PCIBlankEntry},
  {0x80, "Other type of wireless controller", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_0e[] = {
  {0x00, "I2O Architecture", PCIPIFClass_0e00},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_0f[] = {
  {0x01, "TV", PCIBlankEntry},
  {0x02, "Audio", PCIBlankEntry},
  {0x03, "Voice", PCIBlankEntry},
  {0x04, "Data", PCIBlankEntry},
  {0x80, "Other satellite communication controller", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_10[] = {
  {0x00, "Network & computing Encrypt/Decrypt", PCIBlankEntry},
  {0x01, "Entertainment Encrypt/Decrypt", PCIBlankEntry},
  {0x80, "Other Encrypt/Decrypt", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_11[] = {
  {0x00, "DPIO modules", PCIBlankEntry},
  {0x01, "Performance Counters", PCIBlankEntry},
  {0x10, "Communications synchronization plus time and frequency test/measurement ", PCIBlankEntry},
  {0x20, "Management card", PCIBlankEntry},
  {0x80, "Other DAQ & SP controllers", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_12[] = {
  {0x00, "Processing Accelerator", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCISubClass_13[] = {
  {0x00, "Non-Essential Instrumentation Function", PCIBlankEntry},
  {0x00, NULL, NULL}
};

// Programming Interface entries

PciClassEntry PCIPIFClass_0100[] = {
  {0x00, "SCSI controller", PCIBlankEntry},
  {0x11, "SCSI storage device SOP using PQI", PCIBlankEntry},
  {0x12, "SCSI controller SOP using PQI", PCIBlankEntry},
  {0x13, "SCSI storage device and controller SOP using PQI", PCIBlankEntry},
  {0x21, "SCSI storage device SOP using NVMe", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0101[] = {
  {0x00, "", PCIBlankEntry},
  {0x01, "OM-primary", PCIBlankEntry},
  {0x02, "PI-primary", PCIBlankEntry},
  {0x03, "OM/PI-primary", PCIBlankEntry},
  {0x04, "OM-secondary", PCIBlankEntry},
  {0x05, "OM-primary, OM-secondary", PCIBlankEntry},
  {0x06, "PI-primary, OM-secondary", PCIBlankEntry},
  {0x07, "OM/PI-primary, OM-secondary", PCIBlankEntry},
  {0x08, "OM-secondary", PCIBlankEntry},
  {0x09, "OM-primary, PI-secondary", PCIBlankEntry},
  {0x0a, "PI-primary, PI-secondary", PCIBlankEntry},
  {0x0b, "OM/PI-primary, PI-secondary", PCIBlankEntry},
  {0x0c, "OM-secondary", PCIBlankEntry},
  {0x0d, "OM-primary, OM/PI-secondary", PCIBlankEntry},
  {0x0e, "PI-primary, OM/PI-secondary", PCIBlankEntry},
  {0x0f, "OM/PI-primary, OM/PI-secondary", PCIBlankEntry},
  {0x80, "Master", PCIBlankEntry},
  {0x81, "Master, OM-primary", PCIBlankEntry},
  {0x82, "Master, PI-primary", PCIBlankEntry},
  {0x83, "Master, OM/PI-primary", PCIBlankEntry},
  {0x84, "Master, OM-secondary", PCIBlankEntry},
  {0x85, "Master, OM-primary, OM-secondary", PCIBlankEntry},
  {0x86, "Master, PI-primary, OM-secondary", PCIBlankEntry},
  {0x87, "Master, OM/PI-primary, OM-secondary", PCIBlankEntry},
  {0x88, "Master, OM-secondary", PCIBlankEntry},
  {0x89, "Master, OM-primary, PI-secondary", PCIBlankEntry},
  {0x8a, "Master, PI-primary, PI-secondary", PCIBlankEntry},
  {0x8b, "Master, OM/PI-primary, PI-secondary", PCIBlankEntry},
  {0x8c, "Master, OM-secondary", PCIBlankEntry},
  {0x8d, "Master, OM-primary, OM/PI-secondary", PCIBlankEntry},
  {0x8e, "Master, PI-primary, OM/PI-secondary", PCIBlankEntry},
  {0x8f, "Master, OM/PI-primary, OM/PI-secondary", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0105[] = {
  {0x20, "Single stepping", PCIBlankEntry},
  {0x30, "Continuous operation", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0106[] = {
  {0x00, "", PCIBlankEntry},
  {0x01, "AHCI", PCIBlankEntry},
  {0x02, "Serial Storage Bus", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0107[] = {
  {0x00, "", PCIBlankEntry},
  {0x01, "Obsolete", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0108[] = {
  {0x00, "", PCIBlankEntry},
  {0x01, "NVMHCI", PCIBlankEntry},
  {0x02, "NVM Express", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0109[] = {
  {0x00, "", PCIBlankEntry},
  {0x01, "UFSHCI", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0300[] = {
  {0x00, "VGA compatible", PCIBlankEntry},
  {0x01, "8514 compatible", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0604[] = {
  {0x00, "", PCIBlankEntry},
  {0x01, "Subtractive decode", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0609[] = {
  {0x40, "Primary PCI bus side facing the system host processor", PCIBlankEntry},
  {0x80, "Secondary PCI bus side facing the system host processor", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_060b[] = {
  {0x00, "Custom", PCIBlankEntry},
  {0x01, "ASI-SIG Defined Portal", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0700[] = {
  {0x00, "Generic XT-compatible", PCIBlankEntry},
  {0x01, "16450-compatible", PCIBlankEntry},
  {0x02, "16550-compatible", PCIBlankEntry},
  {0x03, "16650-compatible", PCIBlankEntry},
  {0x04, "16750-compatible", PCIBlankEntry},
  {0x05, "16850-compatible", PCIBlankEntry},
  {0x06, "16950-compatible", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0701[] = {
  {0x00, "", PCIBlankEntry},
  {0x01, "Bi-directional", PCIBlankEntry},
  {0x02, "ECP 1.X-compliant", PCIBlankEntry},
  {0x03, "IEEE 1284", PCIBlankEntry},
  {0xfe, "IEEE 1284 target (not a controller)", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0703[] = {
  {0x00, "Generic", PCIBlankEntry},
  {0x01, "Hayes-compatible 16450", PCIBlankEntry},
  {0x02, "Hayes-compatible 16550", PCIBlankEntry},
  {0x03, "Hayes-compatible 16650", PCIBlankEntry},
  {0x04, "Hayes-compatible 16750", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0800[] = {
  {0x00, "Generic 8259", PCIBlankEntry},
  {0x01, "ISA", PCIBlankEntry},
  {0x02, "EISA", PCIBlankEntry},
  {0x10, "IO APIC", PCIBlankEntry},
  {0x20, "IO(x) APIC interrupt controller", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0801[] = {
  {0x00, "Generic 8237", PCIBlankEntry},
  {0x01, "ISA", PCIBlankEntry},
  {0x02, "EISA", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0802[] = {
  {0x00, "Generic 8254", PCIBlankEntry},
  {0x01, "ISA", PCIBlankEntry},
  {0x02, "EISA", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0803[] = {
  {0x00, "Generic", PCIBlankEntry},
  {0x01, "ISA", PCIBlankEntry},
  {0x02, "EISA", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0904[] = {
  {0x00, "Generic", PCIBlankEntry},
  {0x10, "", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0c00[] = {
  {0x00, "", PCIBlankEntry},
  {0x10, "Using 1394 OpenHCI spec", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0c03[] = {
  {0x00, "UHCI", PCIBlankEntry},
  {0x10, "OHCI", PCIBlankEntry},
  {0x20, "EHCI", PCIBlankEntry},
  {0x30, "xHCI", PCIBlankEntry},
  {0x80, "No specific programming interface", PCIBlankEntry},
  {0xfe, "(Not Host Controller)", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0c07[] = {
  {0x00, "SMIC", PCIBlankEntry},
  {0x01, "Keyboard Controller Style", PCIBlankEntry},
  {0x02, "Block Transfer", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0d01[] = {
  {0x00, "Consumer IR controller", PCIBlankEntry},
  {0x10, "UWB Radio controller", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry PCIPIFClass_0e00[] = {
  {0x00, "Message FIFO at offset 40h", PCIBlankEntry},
  {0x01, "", PCIBlankEntry},
  {0x00, NULL, NULL}
};

PciClassEntry* FindClassEntry(uint8_t class, PciClassEntry* entries) {
  int index = 0;
  while (entries[index].Description != NULL) {
    if (entries[index].Code == class) {
      return &entries[index];
    }
    index++;
  }
  return NULL;
}

void pci_get_class_names(pci_device* device, pci_class_cames* names) {
  names->base_class = "?";
  names->sub_class = "?";
  names->pif_class = "?";

  PciClassEntry* base_entry = FindClassEntry(device->base_class_code, gClassStringList);
  if (base_entry != NULL) {
    names->base_class = base_entry->Description;
    PciClassEntry* sub_entry = FindClassEntry(device->sub_class_code, base_entry->LowerLevelClass);
    if (sub_entry != NULL) {
      names->sub_class = sub_entry->Description;
      PciClassEntry* pi_entry = FindClassEntry(device->pi_class_code, sub_entry->LowerLevelClass);
      if (pi_entry != NULL) {
        names->pif_class = pi_entry->Description;
      }
    }
  }
}

void pci_init() {
  pci_mcfg_table* mcfg = find_mcfg_table();
  int bridge_count = (mcfg->header.length - (sizeof(acpi_header) + 8)) / sizeof(pci_host_bridge);
  for (int i = 0; i < bridge_count; i++) {
    pci_host_bridge bridge = mcfg->host_bridge[i];
    mem_range range;
    if (!mem_find_range(bridge.base_address, &range)) {
      printf("Error: cannot find PCI bridge memory range\n");
      return;
    }
    mem_identity_map_range(range.address, range.address + range.size);

    for (int bus = bridge.start_bus_number; bus <= bridge.end_bus_number; bus++) {
      for (int dev = 0; dev <= PCI_MAX_DEVICE; dev++) {
        for (int fun = 0; fun <= PCI_MAX_FUNCTION; fun++) {
          uint64_t addr = bridge.base_address + ((bus - bridge.start_bus_number) << 20) + (dev << 15) + (fun << 12);
          pci_header* header = (pci_header*) addr;
          if (header->vendor_id != 0xffff) { // 0xffff means there is no device
            pci_device* device = &_pci_devices[_pci_device_count++];
            if (_pci_device_count == MAX_PCI_DEVICES) {
              printf("Error: Too many PCI devices, aborting enumeration.\n");
            }
            device->bus_number = bus;
            device->device_number = dev;
            device->function_number = fun;
            device->header_type = header->header_type;
            device->vendor_id = header->vendor_id;
            device->device_id = header->device_id;
            device->base_class_code = header->base_class_code;
            device->sub_class_code = header->sub_class_code;
            device->pi_class_code = header->pi_class_code;
            device->ecam_adress = addr;
            if ((fun == 0) && ((header->header_type & PCI_HEADER_TYPE_MULTI_FUNCTION) == 0x00)) {
              break;
            }
          }
        }
      }
    }
  }
}

void pci_print_mcfg() {
  pci_mcfg_table* mcfg = find_mcfg_table();
  int bridge_count = (mcfg->header.length - (sizeof(acpi_header) + 8)) / sizeof(pci_host_bridge);

  printf("MCFG Addr: 0x%lx\n", (uint64_t) mcfg);
  printf("Signature: %.4s\n", (char*) &(mcfg->header.signature));
  printf("OEM Id: %.6s\n", (char*) &(mcfg->header.oem_id));
  printf("OEM Table Id: %.8s\n", (char*) &(mcfg->header.oem_table_id));
  printf("OEM Revision: %d\n", mcfg->header.oem_revision);
  printf("Length: %d\n", mcfg->header.length);
  printf("PCI Host Bridge Count: %d\n", bridge_count);

  for (int i = 0; i < bridge_count; i++) {
    pci_host_bridge bridge = mcfg->host_bridge[i];
    printf("\nPCI Host Bridge %d\n", i + 1);
    printf("Base Address: 0x%lx\n", bridge.base_address);
    printf("Segment Group Number: %d\n", bridge.segment_group_number);
    printf("Start Bus Number: %d\n", bridge.start_bus_number);
    printf("End Bus Number: %d\n", bridge.end_bus_number);
  }
}

void pci_print_devices() {
  for (int i = 0; i < _pci_device_count; i++) {
    pci_device* device = &_pci_devices[i];
    pci_class_cames strings;
    pci_get_class_names(device, &strings);
    printf("%.2x:%.2x.%d: %x:%.4x %s, %s, %s\n",
           device->bus_number, device->device_number, device->function_number, device->vendor_id, device->device_id,
           strings.base_class, strings.sub_class, strings.pif_class);
  }
}