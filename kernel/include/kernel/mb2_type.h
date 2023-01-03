/*   multiboot2.h - Multiboot 2 header file. */
/*   Copyright (C) 1999,2003,2007,2008,2009,2010  Free Software Foundation, Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL ANY
 *  DEVELOPER OR DISTRIBUTOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef MB2_HEADER
#define MB2_HEADER 1

#include <stdint.h>

/*  How many bytes from the start of the file we search for the header. */
#define MB2_SEARCH                        32768
#define MB2_HEADER_ALIGN                  8

/*  The magic field should contain this. */
#define MB2_HEADER_MAGIC                  0xe85250d6

/*  This should be in %eax. */
#define MB2_BOOTLOADER_MAGIC              0x36d76289

/*  Alignment of multiboot modules. */
#define MB2_MOD_ALIGN                     0x00001000

/*  Alignment of the multiboot info structure. */
#define MB2_INFO_ALIGN                    0x00000008

/*  Flags set in the ’flags’ member of the multiboot header. */

#define MB2_TAG_ALIGN                  8
#define MB2_TAG_TYPE_END               0
#define MB2_TAG_TYPE_CMDLINE           1
#define MB2_TAG_TYPE_BOOT_LOADER_NAME  2
#define MB2_TAG_TYPE_MODULE            3
#define MB2_TAG_TYPE_BASIC_MEMINFO     4
#define MB2_TAG_TYPE_BOOTDEV           5
#define MB2_TAG_TYPE_MMAP              6
#define MB2_TAG_TYPE_VBE               7
#define MB2_TAG_TYPE_FRAMEBUFFER       8
#define MB2_TAG_TYPE_ELF_SECTIONS      9
#define MB2_TAG_TYPE_APM               10
#define MB2_TAG_TYPE_EFI32             11
#define MB2_TAG_TYPE_EFI64             12
#define MB2_TAG_TYPE_SMBIOS            13
#define MB2_TAG_TYPE_ACPI_OLD          14
#define MB2_TAG_TYPE_ACPI_NEW          15
#define MB2_TAG_TYPE_NETWORK           16
#define MB2_TAG_TYPE_EFI_MMAP          17
#define MB2_TAG_TYPE_EFI_BS            18
#define MB2_TAG_TYPE_EFI32_IH          19
#define MB2_TAG_TYPE_EFI64_IH          20
#define MB2_TAG_TYPE_LOAD_BASE_ADDR    21

#define MB2_HEADER_TAG_END  0
#define MB2_HEADER_TAG_INFORMATION_REQUEST  1
#define MB2_HEADER_TAG_ADDRESS  2
#define MB2_HEADER_TAG_ENTRY_ADDRESS  3
#define MB2_HEADER_TAG_CONSOLE_FLAGS  4
#define MB2_HEADER_TAG_FRAMEBUFFER  5
#define MB2_HEADER_TAG_MODULE_ALIGN  6
#define MB2_HEADER_TAG_EFI_BS        7
#define MB2_HEADER_TAG_ENTRY_ADDRESS_EFI32  8
#define MB2_HEADER_TAG_ENTRY_ADDRESS_EFI64  9
#define MB2_HEADER_TAG_RELOCATABLE  10

#define MB2_ARCHITECTURE_I386  0
#define MB2_ARCHITECTURE_MIPS32  4
#define MB2_HEADER_TAG_OPTIONAL 1

#define MB2_LOAD_PREFERENCE_NONE 0
#define MB2_LOAD_PREFERENCE_LOW 1
#define MB2_LOAD_PREFERENCE_HIGH 2

#define MB2_CONSOLE_FLAGS_CONSOLE_REQUIRED 1
#define MB2_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED 2

#ifndef ASM_FILE

typedef struct {
  uint32_t magic;
  uint32_t architecture;
  uint32_t header_length;
  uint32_t checksum;
} mb2_header;

typedef struct {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
} mb2_header_tag;

typedef struct {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
  uint32_t requests[0];
} mb2_header_tag_information_request;

typedef struct {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
  uint32_t header_addr;
  uint32_t load_addr;
  uint32_t load_end_addr;
  uint32_t bss_end_addr;
} mb2_header_tag_address;

typedef struct {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
  uint32_t entry_addr;
} mb2_header_tag_entry_address;

typedef struct {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
  uint32_t console_flags;
} mb2_header_tag_console_flags;

typedef struct {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
} mb2_header_tag_framebuffer;

typedef struct {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
} mb2_header_tag_module_align;

typedef struct {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
  uint32_t min_addr;
  uint32_t max_addr;
  uint32_t align;
  uint32_t preference;
} mb2_header_tag_relocatable;

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} mb2_color;

typedef struct {
  uint64_t addr;
  uint64_t len;
#define MB2_MEMORY_AVAILABLE              1
#define MB2_MEMORY_RESERVED               2
#define MB2_MEMORY_ACPI_RECLAIMABLE       3
#define MB2_MEMORY_NVS                    4
#define MB2_MEMORY_BADRAM                 5
  uint32_t type;
  uint32_t zero;
} mb2_mmap_entry;

typedef struct {
  uint32_t type;
  uint32_t size;
} mb2_tag;

typedef struct {
  uint32_t type;
  uint32_t size;
  char string[0];
} mb2_tag_string;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t mod_start;
  uint32_t mod_end;
  char cmdline[0];
} mb2_tag_module;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t mem_lower;
  uint32_t mem_upper;
} mb2_tag_basic_meminfo;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t biosdev;
  uint32_t slice;
  uint32_t part;
} mb2_tag_bootdev;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t entry_size;
  uint32_t entry_version;
  mb2_mmap_entry entries[0];
} mb2_tag_mmap;

typedef struct {
  uint8_t external_specification[512];
} mb2_vbe_info_block;

typedef struct {
  uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
  uint8_t window_a;			  // deprecated
  uint8_t window_b;			  // deprecated
  uint16_t granularity;		// deprecated; used while calculating bank numbers
  uint16_t window_size;
  uint16_t segment_a;
  uint16_t segment_b;
  uint32_t win_func_ptr;	// deprecated; used to switch banks from protected mode without returning to real mode
  uint16_t pitch;			    // number of bytes per horizontal line
  uint16_t width;			    // width in pixels
  uint16_t height;			  // height in pixels
  uint8_t w_char;			    // unused...
  uint8_t y_char;			    // ...
  uint8_t planes;
  uint8_t bpp;			      // bits per pixel in this mode
  uint8_t banks;		     	// deprecated; length number of banks in this mode
  uint8_t memory_model;
  uint8_t bank_size;	  	// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
  uint8_t image_pages;
  uint8_t reserved0;

  uint8_t red_mask;
  uint8_t red_position;
  uint8_t green_mask;
  uint8_t green_position;
  uint8_t blue_mask;
  uint8_t blue_position;
  uint8_t reserved_mask;
  uint8_t reserved_position;
  uint8_t direct_color_attributes;

  uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
  uint32_t off_screen_mem_off;
  uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
  uint8_t reserved1[206];
} __attribute__ ((packed)) mb2_vbe_mode_info_block;

typedef struct {
  uint32_t type;
  uint32_t size;

  uint16_t vbe_mode;
  /* VBE protected mode interface */
  uint16_t vbe_interface_seg;
  uint16_t vbe_interface_off;
  uint16_t vbe_interface_len;

  mb2_vbe_info_block vbe_control_info;
  mb2_vbe_mode_info_block vbe_mode_info;
} mb2_tag_vbe;

typedef struct {
  uint32_t type;
  uint32_t size;

  uint64_t framebuffer_addr;
  uint32_t framebuffer_pitch;
  uint32_t framebuffer_width;
  uint32_t framebuffer_height;
  uint8_t framebuffer_bpp;
#define MB2_FRAMEBUFFER_TYPE_INDEXED 0
#define MB2_FRAMEBUFFER_TYPE_RGB     1
#define MB2_FRAMEBUFFER_TYPE_EGA_TEXT     2
  uint8_t framebuffer_type;
  uint16_t reserved;
} mb2_tag_framebuffer_common;

typedef struct {
  mb2_tag_framebuffer_common common;
  union {
    struct {
      uint16_t framebuffer_palette_num_colors;
      mb2_color framebuffer_palette[0];
    };
    struct {
      uint8_t framebuffer_red_field_position;
      uint8_t framebuffer_red_mask_size;
      uint8_t framebuffer_green_field_position;
      uint8_t framebuffer_green_mask_size;
      uint8_t framebuffer_blue_field_position;
      uint8_t framebuffer_blue_mask_size;
    };
  };
} mb2_tag_framebuffer;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t num;
  uint32_t entsize;
  uint32_t shndx;
  char sections[0];
} mb2_tag_elf_sections;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint16_t version;
  uint16_t cseg;
  uint32_t offset;
  uint16_t cseg_16;
  uint16_t dseg;
  uint16_t flags;
  uint16_t cseg_len;
  uint16_t cseg_16_len;
  uint16_t dseg_len;
} mb2_tag_apm;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t pointer;
} mb2_tag_efi32;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint64_t pointer;
} mb2_tag_efi64;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint8_t major;
  uint8_t minor;
  uint8_t reserved[6];
  uint8_t tables[0];
} mb2_tag_smbios;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint8_t rsdp[0];
} mb2_tag_old_acpi;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint8_t rsdp[0];
} mb2_tag_new_acpi;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint8_t dhcpack[0];
} mb2_tag_network;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t descr_size;
  uint32_t descr_vers;
  uint8_t efi_mmap[0];
} mb2_tag_efi_mmap;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t pointer;
} mb2_tag_efi32_ih;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint64_t pointer;
} mb2_tag_efi64_ih;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t load_base_addr;
} mb2_tag_load_base_addr;

#endif /*  ! ASM_FILE */

#endif /*  ! MB2_HEADER */
