#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static bool write_tty(__unused char * _, const char * data, size_t length) {
	const unsigned char * bytes = (const unsigned char *) data;
	for (size_t i = 0; i < length; i++) {
		if (putchar(bytes[i]) == EOF) {
			return false;
		}
	}
	return true;
}

static bool write_str(char * str, const char * data, size_t length) {
	return memcpy(str, data, length) != NULL;
}

static int vwprintf(char * str, const char * format, va_list ap, bool (*write)(char *, const char *, size_t)) {
	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;
		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%') {
				amount++;
			}
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!write(str + written, format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		const char * format_begun_at = format++;
		if (*format == 'c') {
			format++;
			char c = (char) va_arg(ap, int);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!write(str + written, &c, sizeof(c)))
				return -1;
			written++;
		} else if (*format == 's') {
			format++;
			const char * strw = va_arg(ap, const char *);
			size_t len = strlen(strw);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!write(str + written, strw, len))
				return -1;
			written += len;
		} else if (*format == 'b') {
			format++;
			int n = va_arg(ap, int);
			char strw[8 * sizeof(int) + 1];
			itoa(n, strw, BINARY);
			size_t len = strlen(strw);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!write(str + written, strw, len))
				return -1;
			written += len;
		} else if (*format == 'o') {
			format++;
			int n = va_arg(ap, int);
			char strw[8 * sizeof(int) + 1];
			itoa(n, strw, OCTAL);
			size_t len = strlen(strw);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!write(str + written, strw, len))
				return -1;
			written += len;
		} else if (*format == 'd') {
			format++;
			int n = va_arg(ap, int);
			char strw[8 * sizeof(int) + 1];
			itoa(n, strw, DECIMAL);
			size_t len = strlen(strw);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!write(str + written, strw, len))
				return -1;
			written += len;
		} else if (*format == 'x') {
			format++;
			int n = va_arg(ap, int);
			char strw[8 * sizeof(int) + 1];
			itoa(n, strw, HEXADECIMAL);
			size_t len = strlen(strw);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!write(str + written, strw, len))
				return -1;
			written += len;
		} else {
			write_tty(NULL, "HIT\n", 4);
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!write(str + written, format, len))
				return -1;
			written += len;
			format += len;
		}
	}

	return written;
}

int vprintf(const char * format, va_list ap) {
	return vwprintf(NULL, format, ap, &write_tty);
}

int vsprintf(char * str, const char * format, va_list ap) {
	int written = vwprintf(str, format, ap, &write_str);
	str[written] = '\0';
	return written;
}

int printf(const char * format, ...) {
	va_list arg;
	int written;

	va_start(arg, format);
	written = vprintf(format, arg);
	va_end(arg);
	
	return written;
}

int sprintf(char * str, const char * format, ...) {
	va_list arg;
	int written;

	va_start(arg, format);
	written = vsprintf(str, format, arg);
	va_end(arg);
	
	return written;
}
