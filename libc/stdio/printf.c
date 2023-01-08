#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <kernel/tty/tty.h>

// Print primitives
#define LEFT_JUSTIFY         1 << 0
#define PREFIX_SIGN          1 << 1
#define PREFIX_BLANK         1 << 2
#define COMMA_TYPE           1 << 3
#define LONG_TYPE            1 << 4
#define PREFIX_ZERO          1 << 6
#define PAD_TO_WIDTH         1 << 9
#define PRECISION            1 << 11
#define ARGUMENT_REVERSED    1 << 12
#define COUNT_ONLY_NO_PRINT  1 << 13
#define UNSIGNED_TYPE        1 << 14

typedef enum {
  RADIX_BIN = 2,
  RADIX_OCT = 8,
  RADIX_DEC = 10,
  RADIX_HEX = 16,
} radix;

//// Define the maximum number of characters that are required to
/// encode with a NULL terminator a decimal, hexadecimal, GUID,
/// or TIME value.
////  Maximum Length Decimal String     = 28
///    "-9,223,372,036,854,775,808"
///  Maximum Length Hexadecimal String = 17
///    "FFFFFFFFFFFFFFFF"
///  Maximum Length GUID               = 37
///    "00000000-0000-0000-0000-000000000000"
///  Maximum Length TIME               = 18
///    "12/12/2006  12:12"
#define MAXIMUM_VALUE_CHARACTERS  38

const char m_hex_str[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

/**
  Internal function that places the character into the Buffer.

  Internal function that places ASCII or Unicode character into the Buffer.

  @param  buffer      The buffer to place the Unicode or ASCII string.
  @param  end_buffer   The end of the input Buffer. No characters will be
                      placed after that.
  @param  length      The count of character to be placed into Buffer.
                      (Negative value indicates no buffer fill.)
  @param  c   The character to be placed into Buffer.
  @param  Increment   The character increment in Buffer.

  @return Buffer.

**/
char* write_char(char* buffer, char* end_buffer, int32_t length, uint32_t c, void (* put_char)(char)) {
  if (put_char != NULL) {
    for (int32_t i = 0; i < length; i++) {
      put_char(c);
    }
  }
  if (buffer != NULL) {
    for (int32_t i = 0; i < length && buffer < end_buffer; i++) {
      *buffer = (char) c;
      buffer++;
    }
  }
  return buffer;
}

/**
  Internal function that convert a number to a string in Buffer.

  @param  buffer    Location to place the ASCII string of Value.
  @param  value     The value to convert to a Decimal or Hexadecimal string in Buffer.
  @param  radix     Radix of the value

  @return A pointer to the end of buffer filled with ASCII string.
**/
char* value_to_string(char* buffer, int64_t value, uint32_t radix) {
  // Loop to convert one digit at a time in reverse order
  *buffer = 0;
  do {
    uint32_t remainder = (uint32_t) ((uint64_t) value % radix);
    value = (uint64_t) value / radix;
    *(++buffer) = m_hex_str[remainder];
  } while (value != 0);

  // Return pointer of the end of filled buffer.
  return buffer;
}

uint32_t sprintf_impl(char* buffer, uint32_t buffer_size, const char* format, VA_LIST va_list, void put_char(char)) {
  char* original_buffer = NULL;
  char* end_buffer = NULL;
  char value_buffer[MAXIMUM_VALUE_CHARACTERS] = {0};
  uint32_t format_char = 0;
  uint32_t width = 0;
  uint32_t precision = 0;
  int64_t value = 0;
  char* argument_string = 0;
  uint32_t argument_character = 0;
  uint32_t count = 0;
  uint32_t index = 0;
  char prefix = 0;
  uint32_t digits = 0;
  radix radix = 0;
  uint32_t length_to_return = 0;
  uint32_t flags = 0;
  bool zero_pad;
  bool comma;
  char character[2];

  // Reserve space for the Null terminator.
  if (buffer != NULL) {
    buffer_size--;
    original_buffer = buffer;

    // Set the tag for the end of the input Buffer.
    end_buffer = buffer + buffer_size;
  }

  // Get the head character from the format string
  format_char = *format;

  // Loop until the end of the format string is reached or the output buffer is full
  while (format_char != 0) {
    if ((buffer != NULL) && (buffer >= end_buffer)) {
      break;
    }

    // Clear all the flag bits except those that may have been passed in
    flags &= (uint32_t) (COUNT_ONLY_NO_PRINT);

    // Set the default width to zero, and the default precision to 1
    width = 0;
    precision = 1;
    prefix = 0;
    comma = false;
    zero_pad = false;
    count = 0;
    digits = 0;

    switch (format_char) {
      case '%':
        // Parse Flags and Width
        for (bool done = false; !done;) {
          format++;
          format_char = *format;
          switch (format_char) {
            case '.':
              flags |= PRECISION;
              break;
            case '-':
              flags |= LEFT_JUSTIFY;
              break;
            case '+':
              flags |= PREFIX_SIGN;
              break;
            case ' ':
              flags |= PREFIX_BLANK;
              break;
            case ',':
              flags |= COMMA_TYPE;
              break;
            case 'L':
            case 'l':
              flags |= LONG_TYPE;
              break;
            case '*':
              if ((flags & PRECISION) == 0) {
                flags |= PAD_TO_WIDTH;
                width = VA_ARG (va_list, uint32_t);
              } else {
                precision = VA_ARG (va_list, uint32_t);
              }
              break;

            case '0'...'9':
              if (format_char == '0' && (flags & PRECISION) == 0) {
                flags |= PREFIX_ZERO;
              }

              for (count = 0; ((format_char >= '0') && (format_char <= '9'));) {
                count = (count * 10) + format_char - '0';
                format++;
                format_char = *format;
              }

              format -= 1;
              if ((flags & PRECISION) == 0) {
                flags |= PAD_TO_WIDTH;
                width = count;
              } else {
                precision = count;
              }
              break;

            case '\0':
              // Make no output if Format string terminates unexpectedly when
              // looking up for flag, width, precision and type.
              format -= 1;
              precision = 0;
              done = true;
              break;

            default:
              done = true;
              break;
          }
        }

        // Handle each argument type
        switch (format_char) {
          case 'p':
          case 'X':
          case 'x':
          case 'o':
          case 'b':
          case 'u':
          case 'd':
          case 'i':
            // 'd', 'u', 'x', and 'X' that are not preceded by 'l' or 'L' are assumed to be type "int".
            // This assumption is made so the format string definition is compatible with the ANSI C
            // Specification for formatted strings.  It is recommended that the Base Types be used
            // everywhere, but in this one case, compliance with ANSI C is more important, and
            // provides an implementation that is compatible with that largest possible set of CPU
            // architectures.  This is why the type "int" is used in this one case.
            switch (format_char) {
              case 'p':
                radix = RADIX_DEC;
                // Flag space, +, 0, L & l are invalid for type p.
                flags &= ~((uint32_t) (PREFIX_BLANK | PREFIX_SIGN | PREFIX_ZERO | LONG_TYPE));
                if (sizeof(void*) > 4) {
                  flags |= LONG_TYPE;
                }
                break;
              case 'X':
                radix = RADIX_DEC;
                flags |= PREFIX_ZERO;
                break;
              case 'x':
                radix = RADIX_HEX;
                comma = false;
                break;
              case 'o':
                radix = RADIX_OCT;
                break;
              case 'b':
                radix = RADIX_BIN;
                break;
              case 'u':
                radix = RADIX_DEC;
                flags &= ~((uint32_t) (PREFIX_SIGN));
                flags |= UNSIGNED_TYPE;
                break;
              case 'd':
              case 'i':
                radix = RADIX_DEC;
                break;
            }

            if ((flags & LONG_TYPE) == 0) {
              value = VA_ARG (va_list, int);
            } else {
              value = VA_ARG (va_list, int64_t);
            }

            if (format_char == 'x' && (flags & LONG_TYPE) == 0 && value < 0) {
              value = (unsigned int) value;
            }

            if ((flags & PREFIX_BLANK) != 0) {
              prefix = ' ';
            }

            if ((flags & PREFIX_SIGN) != 0) {
              prefix = '+';
            }

            if ((flags & COMMA_TYPE) != 0) {
              comma = true;
            }

            if (radix == 10) {
              if (comma) {
                flags &= ~((uint32_t) PREFIX_ZERO);
                precision = 1;
              }

              if ((value < 0) && ((flags & UNSIGNED_TYPE) == 0)) {
                flags |= PREFIX_SIGN;
                prefix = '-';
                value = -value;
              } else if (((flags & UNSIGNED_TYPE) != 0) && ((flags & LONG_TYPE) == 0)) {
                value = (unsigned int) value;
              }
            }

            // Convert Value to a reversed string
            count = value_to_string(value_buffer, value, radix) - value_buffer;
            if ((value == 0) && (precision == 0)) {
              count = 0;
            }

            argument_string = (char*) value_buffer + count;

            digits = count % 3;
            if (digits != 0) {
              digits = 3 - digits;
            }

            if (comma && (count != 0)) {
              count += ((count - 1) / 3);
            }

            if (prefix != 0) {
              count++;
              precision++;
            }

            flags |= ARGUMENT_REVERSED;
            zero_pad = true;
            if ((flags & PREFIX_ZERO) != 0) {
              if ((flags & LEFT_JUSTIFY) == 0) {
                if ((flags & PAD_TO_WIDTH) != 0) {
                  if ((flags & PRECISION) == 0) {
                    precision = width;
                  }
                }
              }
            }
            break;

          case 's':
          case 'S':
            argument_string = (char*) VA_ARG (va_list, char *);
            if (argument_string == NULL) {
              argument_string = "<null string>";
            }
            // Set the default precision for string to be zero if not specified.
            if ((flags & PRECISION) == 0) {
              precision = 0;
            }
            break;

          case 'c':
            character[0] = (uint8_t) VA_ARG (va_list, uint32_t) & 0xFF;
            character[1] = 0;
            argument_string = (char*) character;
            break;

          case '%':
          default:
            // if the type is '%' or unknown, then print it to the screen
            argument_string = (char*) &format_char;
            break;
        }
        break;

      default:
        argument_string = (char*) &format_char;
        break;
    }

    const int32_t argument_string_step = flags & ARGUMENT_REVERSED ? -1 : 1;

    if ((flags & ARGUMENT_REVERSED) == 0) {
      // Compute the number of characters in ArgumentString and store it in Count
      // ArgumentString is either null-terminated, or it contains Precision characters
      for (count = 0; (argument_string[count * argument_string_step] != '\0' ||
          (argument_string_step > 1 && argument_string[count * argument_string_step + 1] != '\0')) &&
          (count < precision || ((flags & PRECISION) == 0)); count++) {
        argument_character = ((argument_string[count * argument_string_step]) |
            ((argument_string[count * argument_string_step + 1]) << 8));
        if (argument_character == 0) {
          break;
        }
      }
    }

    if (precision < count) {
      precision = count;
    }

    // Pad before the string
    if ((flags & (PAD_TO_WIDTH | LEFT_JUSTIFY)) == (PAD_TO_WIDTH)) {
      length_to_return += width - precision;
      if ((flags & COUNT_ONLY_NO_PRINT) == 0) {
        buffer = write_char(buffer, end_buffer, width - precision, ' ', put_char);
      }
    }

    if (zero_pad) {
      if (prefix != 0) {
        length_to_return++;
        if ((flags & COUNT_ONLY_NO_PRINT) == 0) {
          buffer = write_char(buffer, end_buffer, 1, prefix, put_char);
        }
      }

      length_to_return += precision - count;
      if ((flags & COUNT_ONLY_NO_PRINT) == 0) {
        buffer = write_char(buffer, end_buffer, precision - count, '0', put_char);
      }
    } else {
      length_to_return += precision - count;
      if ((flags & COUNT_ONLY_NO_PRINT) == 0) {
        buffer = write_char(buffer, end_buffer, precision - count, ' ', put_char);
      }

      if (prefix != 0) {
        length_to_return++;
        if ((flags & COUNT_ONLY_NO_PRINT) == 0) {
          buffer = write_char(buffer, end_buffer, 1, prefix, put_char);
        }
      }
    }

    // Output the Prefix character if it is present
    index = 0;
    if (prefix != 0) {
      index++;
    }

    // Copy the string into the output buffer performing the required type conversions
    while (index < count && (argument_string[0] != '\0' || (argument_string_step > 1 && argument_string[1] != '\0'))) {
      argument_character = (*argument_string) | (((uint8_t) *(argument_string + 1)) << 8);

      length_to_return++;
      if ((flags & COUNT_ONLY_NO_PRINT) == 0) {
        buffer = write_char(buffer, end_buffer, 1, argument_character, put_char);
      }

      argument_string += argument_string_step;
      index++;
      if (comma) {
        digits++;
        if (digits == 3) {
          digits = 0;
          index++;
          if (index < count) {
            length_to_return++;
            if ((flags & COUNT_ONLY_NO_PRINT) == 0) {
              buffer = write_char(buffer, end_buffer, 1, ',', put_char);
            }
          }
        }
      }
    }

    // Pad after the string
    if ((flags & (PAD_TO_WIDTH | LEFT_JUSTIFY)) == (PAD_TO_WIDTH | LEFT_JUSTIFY)) {
      length_to_return += width - precision;
      if ((flags & COUNT_ONLY_NO_PRINT) == 0) {
        buffer = write_char(buffer, end_buffer, width - precision, ' ', put_char);
      }
    }

    // Get the next character from the format string
    format++;

    // Get the next character from the format string
    format_char = *format;
  }

  if ((flags & COUNT_ONLY_NO_PRINT) != 0) {
    return length_to_return;
  }

  // Null terminate the Unicode or ASCII string
  if (buffer != NULL) {
    write_char(buffer, end_buffer + 1, 1, 0, put_char);
  }

  return buffer - original_buffer;
}

int vprintf(const char* format, VA_LIST ap) {
  return sprintf_impl(NULL, INT16_MAX, format, ap, &tty_putchar);
}

int vsprintf(char* str, const char* format, VA_LIST ap) {
  int written = sprintf_impl(str, INT16_MAX, format, ap, NULL);
  str[written] = '\0';
  return written;
}

int printf(const char* format, ...) {
  VA_LIST marker;
  VA_START (marker, format);
  uint32_t chars_printed = sprintf_impl(NULL, INT16_MAX, format, marker, tty_putchar);
  VA_END (marker);
  return chars_printed;
}

int sprintf(char* buffer, const char* format, ...) {
  VA_LIST marker;
  VA_START (marker, format);
  uint32_t chars_printed = sprintf_impl(buffer, INT16_MAX, format, marker, NULL);
  VA_END (marker);
  return chars_printed;
}
