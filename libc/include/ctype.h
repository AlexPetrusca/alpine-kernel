#ifndef _CTYPE_H
#define _CTYPE_H 1

#include <sys/cdefs.h>

// https://www.qnx.com/developers/docs/6.5.0SP1.update/com.qnx.doc.dinkum_en_c99/ctype.html

__BEGIN_DECLS

static inline int isalpha(int c) {
  return ((unsigned) c | 32) - 'a' < 26;
}

static inline int isascii(int c) {
  return !(c & ~0x7f);
}

static inline int isblank(int c) {
  return c == ' ' || c == '\t';
}

static inline int iscntrl(int c) {
  return (unsigned) c < 0x20 || c == 0x7f;
}

static inline int isdigit(int c) {
  return (unsigned) c - '0' < 10;
}

static inline int isgraph(int c) {
  return (unsigned) c - 0x21 < 0x5e;
}

static inline int islower(int c) {
  return (unsigned) c - 'a' < 26;
}

static inline int isprint(int c) {
  return (unsigned) c - 0x20 < 0x5f;
}

static inline int isalnum(int c) {
  return isalpha(c) || isdigit(c);
}

static inline int ispunct(int c) {
  return isgraph(c) && !isalnum(c);
}

static inline int isspace(int c) {
  return c == ' ' || (unsigned) c - '\t' < 5;
}

static inline int isupper(int c) {
  return (unsigned) c - 'A' < 26;
}

static inline int isxdigit(int c) {
  return isdigit(c) || ((unsigned) c | 32) - 'a' < 6;
}

static inline int tolower(int c) {
  return isupper(c) ? c | 32 : c;
}

static inline int toupper(int c) {
  return islower(c) ? c & 0x5f : c;
}

__END_DECLS

#endif
