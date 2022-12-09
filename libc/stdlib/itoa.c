#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char * itoa(int value, char * str, int base) {
    unsigned int uvalue = value;
    if (uvalue == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    } 
    
    bool isNegative = false;
    if (value < 0 && base == 10) {
        isNegative = true;
        uvalue = -value;
    }

    int i = 0;
    while (uvalue != 0) {
        int rem = uvalue % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        uvalue = uvalue / base;
    }
 
    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0';
    strrev(str);
    return str;
}