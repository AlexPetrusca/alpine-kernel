#include <string.h>

char * strcat(char * dest, const char * src) {
    char * ret = dest;
    while (*dest) {
        dest++;
    }
    while (*src != '\0') {
        *dest++ = *src++;
    }
    return ret;
}

char * strncat(char * dest, const char * src, size_t n) {
    char * ret = dest;
    while (*dest) {
        dest++;
    }
    while (n--) {
        if (*src == '\0') {
            return ret;
        }
        *dest++ = *src++;
    }
    *dest = '\0';
    return ret;
}