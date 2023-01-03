#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <kernel/mem.h>
#include <kernel/acpi.h>

acpi_rsdp* _rsdp;

void acpi_init(mb2_tag_new_acpi* rsdp_tag) {
  assert(rsdp_tag != NULL, "RSDP does not exist");
  _rsdp = (acpi_rsdp*) &rsdp_tag->rsdp;
  mem_range range;
  if (!mem_find_range(_rsdp->rsdt_address, &range)) {
    panic("Cannot find ACPI memory range\n");
  }
  mem_identity_map_range(range.phys_addr, range.size, MEMORY_ACPI);
}

void acpi_print_info(__unused int argc, __unused char** argv) {
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
