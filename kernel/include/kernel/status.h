#ifndef KERNEL_ARCH_X86_64_STATUS_H_
#define KERNEL_ARCH_X86_64_STATUS_H_

typedef enum {
  OK,
  NOT_FOUND
} status;

char* status_get_name(status s);

#define TRY(code)  { status s = code; if (s != OK) return s; }
#define LOG(code)  { status s = code; if (s != OK) printf("Code failed with status %s\n", status_get_name(s)); }

#endif //KERNEL_ARCH_X86_64_STATUS_H_
