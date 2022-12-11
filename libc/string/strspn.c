#include <string.h>

size_t strspn(const char * s1, const char * s2) {
    size_t ret = 0;
    while (*s1 != '\0' && strchr(s2, *s1)) {
        s1++;
        ret++;
    }
    return ret;    
}

size_t strcspn(const char * s1, const char * s2) {
    size_t ret = 0;
    while (*s1 != '\0') {
        if (strchr(s2, *s1)) {
            return ret;
        } else {
            s1++;
            ret++;
        }
    }
    return ret;
}