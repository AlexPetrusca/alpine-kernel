#include <string.h>

bool strequ(const char * s1, const char * s2) {
    return strcmp(s1, s2) == 0;
}


bool strnequ(const char * s1, const char * s2, size_t n) {
    return strncmp(s1, s2, n) == 0;
}