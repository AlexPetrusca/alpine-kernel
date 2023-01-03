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
  uint8_t glyph_width;
  uint8_t glyph_height;
  uint8_t glyph_size;
//  uint32_t num_glyphs;
  char* glyphs;
} psf_font;

psf_font* psf_font_init();
void psf_font_info_print();

#endif //ALPINE_KERNEL_PSF_FONT_H
