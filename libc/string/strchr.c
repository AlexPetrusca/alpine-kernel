#include <string.h>

char * strchr(const char * s, int c) {
    for (; *s != '\0'; s++) {
        if (*s == c) {
            return (char *) s;
        }
    };
    return NULL;
}

char * strrchr(const char * s, int c) {
    char * ret = NULL;
    for (; *s != '\0'; s++)  {
        if (*s == c) {
            ret = (char *) s;
        }
    };
    return ret;
}