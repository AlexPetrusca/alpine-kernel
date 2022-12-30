#include <string.h>
#include <stdio.h>
#include <kernel/acpi.h>
#include <kernel/mem.h>

acpi_rsdp* _rsdp;

void acpi_init(acpi_rsdp* rsdp) {
  _rsdp = rsdp;
  mem_range range;
  if (!mem_find_range(rsdp->rsdt_address, &range)) {
    printf("Error: cannot find ACPI memory range\n");
    return;
  }
  mem_identity_map_range(range.address, range.address + range.size);
}

void acpi_print_info() {
  acpi_rsdt* rsdt = (acpi_rsdt*) (uint64_t) _rsdp->rsdt_address;
  int entry_count = (rsdt->header.length - sizeof(acpi_header)) / 4;
  printf("Signature: %.4s\n", (char*) rsdt);
  printf("ACPI table count: %d\n", entry_count);
  for (int i = 0; i < entry_count; i++) {
    acpi_header* table = (acpi_header*) (uint64_t) rsdt->pointer_to_other_sdt[i];
    printf("Table %d: %.4s, %d bytes\n", i + 1, (char*) table, table->length);
  }
}

acpi_header* acpi_find_table(char* name) {
  acpi_rsdt* rsdt = (acpi_rsdt*) (uint64_t) _rsdp->rsdt_address;
  int entry_count = (rsdt->header.length - sizeof(acpi_header)) / 4;
  for (int i = 0; i < entry_count; i++) {
    acpi_header* table = (acpi_header*) (uint64_t) rsdt->pointer_to_other_sdt[i];
    if (strnequ(name, (char*) table, 4)) {
      return table;
    }
  }
  return NULL;
}
