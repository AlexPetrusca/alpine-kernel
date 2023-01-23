#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <kernel/mem/mem.h>
#include <kernel/device/acpi.h>

acpi_rsdp* _rsdp;
bool acpi_inited;
#define CHECK_INIT(def)  try(acpi_inited, def, "ACPI subsystem not initialized")

void acpi_init(mb2_tag_new_acpi* rsdp_tag) {
  assert(rsdp_tag != NULL, "RSDP does not exist");
  _rsdp = (acpi_rsdp*) &rsdp_tag->rsdp;
  mem_range range;
  assert(mem_find_range(_rsdp->rsdt_address, &range), "Cannot find ACPI memory range\n");
  assert(mem_map_range_4k(range.phys_addr, range.phys_addr, range.size), "Cannot map the ACPI memory range");
  mem_update_range(range.phys_addr, range.phys_addr, range.size, MEMORY_ACPI);
  acpi_inited = true;
}

void acpi_print_info(__unused int argc, __unused char** argv) {
  CHECK_INIT();
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
  CHECK_INIT(NULL);
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
