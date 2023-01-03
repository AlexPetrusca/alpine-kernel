#ifndef _ACPI_H_
#define _ACPI_H_

#include <string.h>
#include <stdint.h>
#include <kernel/mb2_type.h>

// The common ACPI description table header.  This structure prefaces most ACPI tables.
typedef struct {
  uint32_t signature;
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  uint8_t oem_id[6];
  uint64_t oem_table_id;
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
} __attribute__((packed)) acpi_header;

/// Root System Description Pointer Structure
typedef struct {
  uint64_t signature;
  uint8_t checksum;
  uint8_t oem_id[6];
  uint8_t revision;
  uint32_t rsdt_address;
  uint32_t length;
  uint64_t xsdt_address;
  uint8_t extended_checksum;
  uint8_t reserved[3];
} __attribute__((packed)) acpi_rsdp;

typedef struct {
  acpi_header header;
  uint32_t pointer_to_other_sdt[];
} __attribute__((packed)) acpi_rsdt;

void acpi_init(mb2_tag_new_acpi* rsdp_tag);
acpi_header* acpi_find_table(char* name);

void acpi_print_info(int argc, char** argv);

#endif //_ACPI_H_
