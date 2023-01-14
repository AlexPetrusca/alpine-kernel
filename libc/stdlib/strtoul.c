#include <stdlib.h>

unsigned long int strtoul(const char* nptr, char** endptr, int base) {
  return (unsigned long int) strtoumax(nptr, endptr, base);
}

unsigned long long int strtoull(const char* nptr, char** endptr, int base) {
  return (unsigned long long int) strtoumax(nptr, endptr, base);
}
