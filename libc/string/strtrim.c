#include <string.h>
#include <ctype.h>

char* strtrim(char* s) {
  char* leading = strtrim_leading(s);
  return strtrim_trailing(leading);
}

char* strtrim_leading(char* s) {
  int i = 0;
  while (isspace(s[i])) {
    i++;
  }
  return s + i;
}

char* strtrim_trailing(char* s){
  size_t i = strlen(s) - 1;
  while (isspace(s[i])) {
    i--;
  }
  s[i + 1] = '\0';
  return s;
}