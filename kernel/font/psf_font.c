#include <string.h>
#include <stdio.h>

#include <kernel/font/psf_font.h>

// https://www.win.tue.nl/~aeb/linux/kbd/font-formats-1.html

typedef struct {
  uint16_t magic;             /* Magic number */
  uint8_t mode;               /* PSF font mode */
  uint8_t charsize;           /* Character size */
} psf1_header;

typedef struct {
  uint32_t magic;
  uint32_t version;
  uint32_t headersize;    /* offset of bitmaps in file */
  uint32_t flags;
  uint32_t length;        /* number of glyphs */
  uint32_t charsize;      /* number of bytes for each character */
  uint32_t height;
  uint32_t width;
  /* charsize = tty_height * ((tty_width + 7) / 8) */
} psf2_header;

typedef union {
  psf1_header psf1;
  psf2_header psf2;
} psf_header;

extern psf_header _binary_font_Default_psf_start;
extern psf_header _binary_font_Fixed13_psf_start;
extern psf_header _binary_font_Fixed14_psf_start;
extern psf_header _binary_font_Fixed15_psf_start;
extern psf_header _binary_font_Fixed16_psf_start;
extern psf_header _binary_font_Fixed18_psf_start;
extern psf_header _binary_font_Terminus12x6_psf_start;
extern psf_header _binary_font_Terminus14_psf_start;
extern psf_header _binary_font_Terminus16_psf_start;
extern psf_header _binary_font_Terminus18x10_psf_start;
extern psf_header _binary_font_Terminus20x10_psf_start;
extern psf_header _binary_font_Terminus22x11_psf_start;
extern psf_header _binary_font_Terminus24x12_psf_start;
extern psf_header _binary_font_Terminus28x14_psf_start;
extern psf_header _binary_font_Terminus32x16_psf_start;
extern psf_header _binary_font_TerminusBold14_psf_start;
extern psf_header _binary_font_TerminusBold16_psf_start;
extern psf_header _binary_font_TerminusBold18x10_psf_start;
extern psf_header _binary_font_TerminusBold20x10_psf_start;
extern psf_header _binary_font_TerminusBold22x11_psf_start;
extern psf_header _binary_font_TerminusBold24x12_psf_start;
extern psf_header _binary_font_TerminusBold28x14_psf_start;
extern psf_header _binary_font_TerminusBold32x16_psf_start;
extern psf_header _binary_font_TerminusBoldVGA14_psf_start;
extern psf_header _binary_font_TerminusBoldVGA16_psf_start;

psf_fonts psf_available_fonts;
psf_font* psf_active_font;

void psf_register_font(psf_font* fcontainer, psf_header* fheader) {
  if (fheader->psf1.magic == PSF1_MAGIC) {
    psf1_header* psf1 = &fheader->psf1;
    fcontainer->psf_version = 1;
    fcontainer->psf_header = (char*) psf1;
    fcontainer->glyph_width = PSF1_CHAR_WIDTH;
    fcontainer->glyph_height = psf1->charsize;
    fcontainer->glyph_size = psf1->charsize;
    fcontainer->num_glyphs = (psf1->mode & PSF1_MODE512) ? 512 : 256;
    fcontainer->glyphs = (char*) psf1 + sizeof(psf1_header);
  } else if (fheader->psf2.magic == PSF2_MAGIC) {
    psf2_header* psf2 = &fheader->psf2;
    fcontainer->psf_version = 2;
    fcontainer->psf_header = (char*) psf2;
    fcontainer->glyph_width = psf2->width;
    fcontainer->glyph_height = psf2->height;
    fcontainer->glyph_size = psf2->charsize;
    fcontainer->num_glyphs = psf2->length;
    fcontainer->glyphs = (char*) psf2 + psf2->headersize;
    for (uint32_t i = 0; i < psf2->length; i++) {
      char* glyph = fcontainer->glyphs + i * psf2->charsize;
      for (uint32_t y = 0; y < 2 * psf2->height; y += 2) {
        char temp = glyph[y];
        glyph[y] = glyph[y + 1];
        glyph[y + 1] = temp;
      }
    }
  }
}

psf_font* psf_init_fonts() {
  psf_register_font(&psf_available_fonts.Default, &_binary_font_Default_psf_start);
  psf_register_font(&psf_available_fonts.Fixed13, &_binary_font_Fixed13_psf_start);
  psf_register_font(&psf_available_fonts.Fixed14, &_binary_font_Fixed14_psf_start);
  psf_register_font(&psf_available_fonts.Fixed15, &_binary_font_Fixed15_psf_start);
  psf_register_font(&psf_available_fonts.Fixed16, &_binary_font_Fixed16_psf_start);
  psf_register_font(&psf_available_fonts.Fixed18, &_binary_font_Fixed18_psf_start);
  psf_register_font(&psf_available_fonts.Terminus12x6, &_binary_font_Terminus12x6_psf_start);
  psf_register_font(&psf_available_fonts.Terminus14, &_binary_font_Terminus14_psf_start);
  psf_register_font(&psf_available_fonts.Terminus16, &_binary_font_Terminus16_psf_start);
  psf_register_font(&psf_available_fonts.Terminus18x10, &_binary_font_Terminus18x10_psf_start);
  psf_register_font(&psf_available_fonts.Terminus20x10, &_binary_font_Terminus20x10_psf_start);
  psf_register_font(&psf_available_fonts.Terminus22x11, &_binary_font_Terminus22x11_psf_start);
  psf_register_font(&psf_available_fonts.Terminus24x12, &_binary_font_Terminus24x12_psf_start);
  psf_register_font(&psf_available_fonts.Terminus28x14, &_binary_font_Terminus28x14_psf_start);
  psf_register_font(&psf_available_fonts.Terminus32x16, &_binary_font_Terminus32x16_psf_start);
  psf_register_font(&psf_available_fonts.TerminusBold14, &_binary_font_TerminusBold14_psf_start);
  psf_register_font(&psf_available_fonts.TerminusBold16, &_binary_font_TerminusBold16_psf_start);
  psf_register_font(&psf_available_fonts.TerminusBold18x10, &_binary_font_TerminusBold18x10_psf_start);
  psf_register_font(&psf_available_fonts.TerminusBold20x10, &_binary_font_TerminusBold20x10_psf_start);
  psf_register_font(&psf_available_fonts.TerminusBold22x11, &_binary_font_TerminusBold22x11_psf_start);
  psf_register_font(&psf_available_fonts.TerminusBold24x12, &_binary_font_TerminusBold24x12_psf_start);
  psf_register_font(&psf_available_fonts.TerminusBold28x14, &_binary_font_TerminusBold28x14_psf_start);
  psf_register_font(&psf_available_fonts.TerminusBold32x16, &_binary_font_TerminusBold32x16_psf_start);
  psf_register_font(&psf_available_fonts.TerminusBoldVGA14, &_binary_font_TerminusBoldVGA14_psf_start);
  psf_register_font(&psf_available_fonts.TerminusBoldVGA16, &_binary_font_TerminusBoldVGA16_psf_start);
  psf_active_font = &psf_available_fonts.Terminus18x10;
  return psf_active_font;
}

psf_font* psf_name2font(char* name) {
  if (strequ(name, "Default")) {
    return &psf_available_fonts.Default;
  } else if (strequ(name, "Fixed13")) {
    return &psf_available_fonts.Fixed13;
  } else if (strequ(name, "Fixed14")) {
    return &psf_available_fonts.Fixed14;
  } else if (strequ(name, "Fixed15")) {
    return &psf_available_fonts.Fixed15;
  } else if (strequ(name, "Fixed16")) {
    return &psf_available_fonts.Fixed16;
  } else if (strequ(name, "Fixed18")) {
    return &psf_available_fonts.Fixed18;
  } else if (strequ(name, "Terminus12x6")) {
    return &psf_available_fonts.Terminus12x6;
  } else if (strequ(name, "Terminus14")) {
    return &psf_available_fonts.Terminus14;
  } else if (strequ(name, "Terminus16")) {
    return &psf_available_fonts.Terminus16;
  } else if (strequ(name, "Terminus18x10")) {
    return &psf_available_fonts.Terminus18x10;
  } else if (strequ(name, "Terminus20x10")) {
    return &psf_available_fonts.Terminus20x10;
  } else if (strequ(name, "Terminus22x11")) {
    return &psf_available_fonts.Terminus22x11;
  } else if (strequ(name, "Terminus24x12")) {
    return &psf_available_fonts.Terminus24x12;
  } else if (strequ(name, "Terminus28x14")) {
    return &psf_available_fonts.Terminus28x14;
  } else if (strequ(name, "Terminus32x16")) {
    return &psf_available_fonts.Terminus32x16;
  } else if (strequ(name, "TerminusBold14")) {
    return &psf_available_fonts.TerminusBold14;
  } else if (strequ(name, "TerminusBold16")) {
    return &psf_available_fonts.TerminusBold16;
  } else if (strequ(name, "TerminusBold18x10")) {
    return &psf_available_fonts.TerminusBold18x10;
  } else if (strequ(name, "TerminusBold20x10")) {
    return &psf_available_fonts.TerminusBold20x10;
  } else if (strequ(name, "TerminusBold22x11")) {
    return &psf_available_fonts.TerminusBold22x11;
  } else if (strequ(name, "TerminusBold24x12")) {
    return &psf_available_fonts.TerminusBold24x12;
  } else if (strequ(name, "TerminusBold28x14")) {
    return &psf_available_fonts.TerminusBold28x14;
  } else if (strequ(name, "TerminusBold32x16")) {
    return &psf_available_fonts.TerminusBold32x16;
  } else if (strequ(name, "TerminusBoldVGA14")) {
    return &psf_available_fonts.TerminusBoldVGA14;
  } else if (strequ(name, "TerminusBoldVGA16")) {
    return &psf_available_fonts.TerminusBoldVGA16;
  }
  return NULL;
}

void psf_set_active_font(psf_font* font) {
  psf_active_font = font;
}

psf_font* psf_get_active_font() {
  return psf_active_font;
}

psf_fonts* psf_get_available_fonts() {
  return &psf_available_fonts;
}

void psf_font_info_print_x(psf_font f) {
  printf("PSF version: %d\n", f.psf_version);
  if (f.psf_version == 1) {
    psf1_header* psf1 = (psf1_header*) f.psf_header;
    printf("PSF1 magic: 0x%x\n", psf1->magic);
    printf("PSF1 flags: %d\n", psf1->mode);
    printf("Character tty_width: %d\n", PSF1_CHAR_WIDTH);
    printf("Character tty_height: %d\n", psf1->charsize);
    printf("Character size (in bytes): %d\n", psf1->charsize);
  } else if (f.psf_version == 2) {
    psf2_header* psf2 = (psf2_header*) f.psf_header;
    printf("PSF2 magic: 0x%x\n", psf2->magic);
    printf("PSF2 flags: %d\n", psf2->flags);
    printf("PSF2 headersize: %d\n", psf2->headersize);
    printf("Number of glyphs: %d\n", psf2->length);
    printf("Character tty_width: %d\n", psf2->width);
    printf("Character tty_height: %d\n", psf2->height);
    printf("Character size (in bytes): %d\n", psf2->charsize);
  }
  printf("Sample glyph bitmap (\"W\"): \n");
  char* glyph = f.glyphs + 'W' * f.glyph_size;
  for (uint32_t i = 0; i < f.glyph_height; i++) {
    if (f.glyph_width <= PSF1_CHAR_WIDTH) {
      printf(" %.8b\n", (uint8_t) glyph[i]);
    } else {
      printf(" %.16b\n", ((uint16_t*) glyph)[i]);
    }
  }
}

void psf_font_info_print(int argc, char** argv) {
  if (argc > 1) {
    psf_font* font = psf_name2font(argv[1]);
    if (font) {
      psf_font_info_print_x(*font);
    } else {
      printf("Error: font %s not found", argv[1]);
    }
  } else {
    psf_font_info_print_x(*psf_active_font);
  }
}