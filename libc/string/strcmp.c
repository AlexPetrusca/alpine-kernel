#include <string.h>

int strcmp(const char * s1, const char * s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *) s1 - *(const unsigned char *) s2;
}

int strncmp(const char * s1, const char * s2, size_t n) {
    for (size_t i = 0; i < n && *s1 && (*s1 == *s2); i++) {
        s1++;
        s2++;
    }
    return *(const unsigned char *) s1 - *(const unsigned char *) s2;
}