#include <stddef.h>
#include <stdio.h>

#include <kernel/psf_font.h>

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
  /* charsize = height * ((width + 7) / 8) */
} psf2_header;

extern psf2_header _binary_arch_x86_64_font_psf_start;
extern psf2_header _binary_arch_x86_64_font2_psf_start;
extern psf2_header _binary_arch_x86_64_font3_psf_start;
extern psf2_header _binary_arch_x86_64_font4_psf_start;
extern psf2_header _binary_arch_x86_64_font5_psf_start;

psf_font font;

psf_font* psf_font_init() {
  psf1_header* psf1 = (psf1_header*) &_binary_arch_x86_64_font5_psf_start;
  if (psf1->magic == PSF1_MAGIC) {
    font.psf_version = 1;
    font.glyph_width = PSF1_CHAR_WIDTH;
    font.glyph_height = psf1->charsize;
    font.glyph_size = psf1->charsize;
    font.glyphs = (char*) psf1 + sizeof(psf1_header);
    return &font;
  }

  psf2_header* psf2 = (psf2_header*) &_binary_arch_x86_64_font5_psf_start;
  if (psf2->magic == PSF2_MAGIC) {
    font.psf_version = 2;
    font.glyph_width = psf2->width;
    font.glyph_height = psf2->height;
    font.glyph_size = psf2->charsize;
    font.glyphs = (char*) psf2 + psf2->headersize;
    for (int i = 0; i < psf2->length; i++) {
      char* glyph = font.glyphs + i * psf2->charsize;
      for (int y = 0; y < 2 * psf2->height; y += 2) {
        char temp = glyph[y];
        glyph[y] = glyph[y + 1];
        glyph[y + 1] = temp;
      }
    }
    return &font;
  }

  return NULL;
}

void psf_font_info_print() {
//  printf("PSF version: %d\n", font.psf_version);
//  if (font.psf_version == 1) {
//    psf1_header* psf1 = (psf1_header*) &_binary_arch_x86_64_font_psf_start;
//    printf("PSF1 magic: 0x%x\n", psf1->magic);
//    printf("PSF1 flags: %d\n", psf1->mode);
//    printf("Character width: %d\n", PSF1_CHAR_WIDTH);
//    printf("Character height: %d\n", psf1->charsize);
//    printf("Character size (in bytes): %d\n", psf1->charsize);
//  } else if (font.psf_version == 2) {
//    psf2_header* psf2 = (psf2_header*) &_binary_arch_x86_64_font_psf_start;
//    printf("PSF2 magic: 0x%x\n", psf2->magic);
//    printf("PSF2 flags: %d\n", psf2->flags);
//    printf("PSF2 headersize: %d\n", psf2->headersize);
//    printf("Number of glyphs: %d\n", psf2->length);
//    printf("Character width: %d\n", psf2->width);
//    printf("Character height: %d\n", psf2->height);
//    printf("Character size (in bytes): %d\n", psf2->charsize);
//  } else {
//    printf("Invalid font binary.");
//  }

  psf2_header* psf2 = (psf2_header*) &_binary_arch_x86_64_font5_psf_start;
  printf("PSF2 magic: 0x%x\n", psf2->magic);
  printf("PSF2 flags: %d\n", psf2->flags);
  printf("PSF2 headersize: %d\n", psf2->headersize);
  printf("Number of glyphs: %d\n", psf2->length);
  printf("Character width: %d\n", psf2->width);
  printf("Character height: %d\n", psf2->height);
  printf("Character size (in bytes): %d\n", psf2->charsize);

  char* glyphs = (char*) psf2 + psf2->headersize;
  char* glyph = glyphs + 'W' * psf2->charsize;

  for (int i = 0; i < psf2->height; i++) {
    printf("%.16b\n", ((uint16_t*) glyph)[i]);
  }
}