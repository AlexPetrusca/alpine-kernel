#ifndef ALPINE_KERNEL_PSF_FONT_H
#define ALPINE_KERNEL_PSF_FONT_H

#include <stdint.h>

#define PSF1_MAGIC        0x0436
#define PSF2_MAGIC        0x864AB572

#define PSF1_MODE512      0x01
#define PSF1_MODEHASTAB   0x02
#define PSF1_MODEHASSEQ   0x04
#define PSF1_MAXMODE      0x05
#define PSF2_FLAGSHASUNI  0x01

#define PSF1_CHAR_WIDTH   8
#define PSF2_CHAR_WIDTH   16

typedef struct {
  uint8_t psf_version;
  char* psf_header;
  uint8_t glyph_width;
  uint8_t glyph_height;
  uint32_t glyph_size;
  uint32_t num_glyphs;
  char* glyphs;
} psf_font;

typedef struct {
  psf_font Default;
  psf_font Fixed13;
  psf_font Fixed14;
  psf_font Fixed15;
  psf_font Fixed16;
  psf_font Fixed18;
  psf_font Terminus12x6;
  psf_font Terminus14;
  psf_font Terminus16;
  psf_font Terminus18x10;
  psf_font Terminus20x10;
  psf_font Terminus22x11;
  psf_font Terminus24x12;
  psf_font Terminus28x14;
  psf_font Terminus32x16;
  psf_font TerminusBold14;
  psf_font TerminusBold16;
  psf_font TerminusBold18x10;
  psf_font TerminusBold20x10;
  psf_font TerminusBold22x11;
  psf_font TerminusBold24x12;
  psf_font TerminusBold28x14;
  psf_font TerminusBold32x16;
  psf_font TerminusBoldVGA14;
  psf_font TerminusBoldVGA16;
} psf_fonts;

psf_font* psf_init_fonts();
psf_font* psf_get_active_font();
psf_font* psf_name2font(char* name);
void psf_set_active_font(psf_font* font);
psf_fonts* psf_get_available_fonts();
void psf_font_info_print();

#endif //ALPINE_KERNEL_PSF_FONT_H
