#ifndef _KERNEL_SH_H
#define _KERNEL_SH_H

typedef struct _sh_command {
  char* name;
  void (* run)();
} sh_command;

void shell_start(sh_command cmd[]);

#endif