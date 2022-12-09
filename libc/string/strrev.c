#include <string.h>

char * strrev(char * str) {
    char ch;
    int i = strlen(str) - 1, j = 0;
    while (i > j) {
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
        i--;
        j++;
    }
    return str;
}