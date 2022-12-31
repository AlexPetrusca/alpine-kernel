#ifndef ALPINE_KERNEL_PSF_FONT_H
#define ALPINE_KERNEL_PSF_FONT_H

#include <stdint.h>

#define PSF_MAGIC        0x0436

#define PSF_MODE512      0x01
#define PSF_MODEHASTAB   0x02
#define PSF_MODEHASSEQ   0x04
#define PSF_MAXMODE      0x05

#define PSF_SEPARATOR    0xFFFF
#define PSF_STARTSEQ     0xFFFE

#define PSF_NUM_GLYPHS   256
#define PSF_CHAR_WIDTH   8

extern char _binary_arch_x86_64_font_psf_start;

typedef struct {
  uint8_t rows[9];
} psf_glyph9;

typedef struct {
  uint8_t rows[16];
} psf_glyph16;

typedef union {
  struct {
    psf_glyph9 glyphs9[PSF_NUM_GLYPHS];
  };
  struct {
    psf_glyph16 glyphs16[PSF_NUM_GLYPHS];
  };
} psf_glyphs;

typedef struct {
  uint16_t magic;     /* Magic number */
  uint8_t mode;       /* PSF vbe_tty_font mode */
  uint8_t charsize;   /* Character size */
  psf_glyphs glyphs;  /* List of PSF Glyphs */
} psf_font;

psf_font * psf_font_get_font(psf_font** font) {
  *font = (psf_font*) &_binary_arch_x86_64_font_psf_start;
  return *font;
}

#endif //ALPINE_KERNEL_PSF_FONT_H
