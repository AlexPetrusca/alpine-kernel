#include <stdio.h>

#include <kernel/mb2_info.h>

mb2_info mbi;

mb2_info* mb2_get_tags() {
  return &mbi;
}

mb2_info* mb2_info_init(uint64_t kernel_addr) {
  for (mb2_tag* tag = (mb2_tag*) (kernel_addr + 8);
       tag->type != MB2_TAG_TYPE_END;
       tag = (mb2_tag*) ((uint8_t*) tag + ((tag->size + 7) & ~7))) {
    switch (tag->type) {
      case MB2_TAG_TYPE_CMDLINE:
        mbi.kernel_cmd_line = ((mb2_tag_string*) tag)->string;
        break;

      case MB2_TAG_TYPE_BOOT_LOADER_NAME:
        mbi.bootloader_name = ((mb2_tag_string*) tag)->string;
        break;

      case MB2_TAG_TYPE_BASIC_MEMINFO:
        mbi.basic_meminfo_tag = (mb2_tag_basic_meminfo*) tag;
        break;

      case MB2_TAG_TYPE_MMAP:
        mbi.mem_map_tag = (mb2_tag_mmap*) tag;
        break;

      case MB2_TAG_TYPE_VBE:
        mbi.vbe_tag = (mb2_tag_vbe*) tag;
        break;

      case MB2_TAG_TYPE_BOOTDEV:
        mbi.bootdev_tag = (mb2_tag_bootdev*) tag;
        break;

      case MB2_TAG_TYPE_FRAMEBUFFER:
        mbi.framebuffer_tag = (mb2_tag_framebuffer*) tag;
        break;

      case MB2_TAG_TYPE_LOAD_BASE_ADDR:
        mbi.kernel_base_addr = ((mb2_tag_load_base_addr*) tag)->load_base_addr;
        break;

      case MB2_TAG_TYPE_APM: {
        mbi.apm_tag = (mb2_tag_apm*) tag;
        break;
      }

      case MB2_TAG_TYPE_ELF_SECTIONS: {
        mbi.elf_sections_tag = (mb2_tag_elf_sections*) tag;
        break;
      }

      case MB2_TAG_TYPE_ACPI_OLD:
      case MB2_TAG_TYPE_ACPI_NEW:
        mbi.rsdp_tag = (mb2_tag_new_acpi*) tag;
        break;

      default:
//        printf("Error: Unknown tag %d\n", tag->type);
    }
  }
  return mb2_get_tags();
}

void mb2_info_print(__unused int argc, __unused char** argv) {
  printf("Boot loader name = %s\n", mbi.bootloader_name);
  printf("Boot device 0x%x,%u,%u\n", mbi.bootdev_tag->biosdev, mbi.bootdev_tag->slice, mbi.bootdev_tag->part);
  printf("Load base addr 0x%lx\n", mbi.kernel_base_addr);
  printf("APM cseg=%d:%d, dseg=%d:%d\n", mbi.apm_tag->cseg, mbi.apm_tag->cseg_len, mbi.apm_tag->dseg,
         mbi.apm_tag->dseg_len);
  printf("ELF sections: %d\n", mbi.elf_sections_tag->num);
  printf("Screen %d x %d\n", mbi.framebuffer_tag->common.framebuffer_width,
         mbi.framebuffer_tag->common.framebuffer_height);
  printf("Command line = %s\n", mbi.kernel_cmd_line);
}

void mb2_fb_info_print(__unused int argc, __unused char** argv) {
  mb2_tag_framebuffer_common common = mbi.framebuffer_tag->common;
  if (common.framebuffer_type == MB2_FRAMEBUFFER_TYPE_RGB) {
    printf("Type: VBE RGB Mode\n");
    printf("VBE Mode: 0x%x\n", mbi.vbe_tag->vbe_mode);
    printf("VBE Control Info: %s\n", mbi.vbe_tag->vbe_control_info.external_specification);
  } else {
    printf("Type: VGA Text Mode\n");
  }
  uint64_t fb_size = (common.framebuffer_width * common.framebuffer_height) * sizeof(uint32_t);
  printf("FB Address: 0x%.12lx - 0x%.12lx\n", common.framebuffer_addr, common.framebuffer_addr + fb_size);
  printf("Width: %d\n", common.framebuffer_width);
  printf("Height: %d\n", common.framebuffer_height);
  printf("Bits Per Pixel: %d\n", common.framebuffer_bpp);
}