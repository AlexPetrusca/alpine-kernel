#include <stdlib.h>

long int strtol(const char* nptr, char** endptr, int base) {
  return (long int) strtoimax(nptr, endptr, base);
}

long long int strtoll(const char* nptr, char** endptr, int base) {
  return (long long int) strtoimax(nptr, endptr, base);
}