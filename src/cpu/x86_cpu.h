// Tensor VM - cpu/x86_cpu.h

#ifndef X86_CPU_H
#define X86_CPU_H

#include <stdlib.h>

#include "register.h"

int cpu_init(void);
void cpu_reset(void);
void show_reg(void);

extern reg_p reg;
inline void cpu_destroy(void) {free(reg);}

void exec(void);
int instr_parse(uint16_t);

#endif
