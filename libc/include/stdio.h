#ifndef _STDIO_H
#define _STDIO_H 1

#include <stdarg.h>

#include <sys/cdefs.h>

#define EOF -1

/**
  The print functions provide a simple means to produce formatted output
  strings. The format of argument descriptors is described below:

    %[flags][width][.precision]type

  [flags]:
    - -
      - The field is left justified.  If not specified, then the field is right justified.
    - space
      - Prefix a space character to a number.  Only valid for types X, x, i and d.
    - +
      - Prefix a plus character to a number.  Only valid for types X, x, and d.
        If both space and + are specified, then space is ignored.
    - 0
      - Pad with 0 characters to the left of a number.  Only valid for X, x, and d.
    - ,
      - Place a comma every 3rd digit of the number.  Only valid for i and d.
        If 0 is also specified, then 0 is ignored.
    - L, l
      - The number being printed is size uint64_t.  Only valid for types X, x, and d.
        If this flag is not specified, then the number being printed is size int.
    - NOTE: All invalid flags are ignored.

  [width]:

    - *
      - The width of the field is specified by a uint32_t argument in the
        argument list.
    - number
      - The number specified as a decimal value represents the width of
        the field.
    - NOTE: If [width] is not specified, then a field width of 0 is assumed.

  [.precision]:

    - *
      - The precision of the field is specified by a uint32_t argument in the argument list.
    - number
      - The number specified as a decimal value represents the precision of the field.

    - NOTE: If [.precision] is not specified, then a precision of 0 is assumed.

  type:
    - %
      - Print a %%.
    - d
      - The argument is a signed decimal number.  If the flag 'L' is not specified,
        then the argument is assumed to be size int.
    - u
      - The argument is a unsigned decimal number.  If the flag 'L' is not specified,
        then the argument is assumed to be size int.
    -f,F
      - not supported
    -e,E
      - not supported
    -g,G
      - not supported
    - a, A
      - not supported
    - x
      - The argument is an unsigned hexadecimal number.  The characters used are 0..9 and
        A..F.  If the flag 'L' is not specified, then the argument is assumed
        to be size int.  This does not follow ANSI C.
    - X
      - The argument is an unsigned hexadecimal number and the number is padded with
        zeros.  This is equivalent to a format string of "0x". If the flag
        'L' is not specified, then the argument is assumed to be size int.
        This does not follow ANSI C.
    -o
      - The argument is an unsigned octal number.
    - s
      - The argument is a pointer to a string.
    - c
      - The argument is a Unicode character.  ASCII characters can be printed
        using this type too by making sure bits 8..15 of the argument are set to 0.
    - p
      - The argument is a pointer that is a (VOID *), and it is printed as an
        unsigned hexadecimal number  The characters used are 0..9 and A..F.
    - n
      - not supported

  @param[out] Buffer          The character buffer to print the results of the
                              parsing of Format into.
  @param[in]  buffer_size      The maximum number of characters to put into
                              buffer.
  @param[in]  Flags           Initial flags value.
                              Can only have FORMAT_UNICODE, OUTPUT_UNICODE,
                              and COUNT_ONLY_NO_PRINT set.
  @param[in]  format          A Null-terminated format string.
  @param[in]  VaListMarker    VA_LIST style variable argument list consumed by
                              processing Format.
  @param[in]  BaseListMarker  BASE_LIST style variable argument list consumed
                              by processing Format.

  @return The number of characters printed not including the Null-terminator.
          If COUNT_ONLY_NO_PRINT was set returns the same, but without any
          modification to Buffer.
**/

__BEGIN_DECLS

int printf(const char* format, ...);
int sprintf(char* str, const char* format, ...);
int _sprintf(char* buffer, const char* format, ...);
int vprintf(const char* format, va_list ap);
int vsprintf(char* str, const char* format, va_list ap);

int getchar();
int putchar(int c);
int puts(const char* s);

__END_DECLS

#endif
