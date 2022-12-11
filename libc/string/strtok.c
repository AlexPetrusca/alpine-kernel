#include <string.h>

char * strtok(char * str, const char * delim) {
    static char * p = NULL;
    if (str != NULL) {
        p = str;
    } else if (p == NULL) {
        return NULL;
    }
    str = p + strspn(p, delim);
    p = str + strcspn(str, delim);
    if (p == str) {
        return p = NULL;
    } else if (*p != '\0') {
        *p = '\0';
        p++;
    } else {
        p = NULL;
    };
    return str;
}

char * strtok_r(char * str, const char * delim, char ** saveptr) {
    if (str != NULL) {
        *saveptr = str;
    } else if (*saveptr == NULL) {
        return NULL;
    }
    str = *saveptr + strspn(*saveptr, delim);
    *saveptr = str + strcspn(str, delim);
    if (*saveptr == str) {
        return *saveptr = NULL;
    } else if (**saveptr != '\0') {
        **saveptr = '\0';
        (*saveptr)++;
    } else {
        *saveptr = NULL;
    };
    return str;
}