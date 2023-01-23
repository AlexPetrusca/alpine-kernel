#ifndef KERNEL_CPU_SMP_H_
#define KERNEL_CPU_SMP_H_

#include <stdnoreturn.h>

void smp_init();
extern noreturn void enter_smp();

#endif //KERNEL_CPU_SMP_H_
