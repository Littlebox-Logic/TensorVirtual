// Tensor VM - cpu/x86_cpu.h

#ifndef X86_CPU_H
#define X86_CPU_H

#include <stdlib.h>

#include "register.h"

int cpu_init(void);
void cpu_reset(void);
void show_reg(void);
void rom_int(void);

extern reg_p reg;
inline void cpu_destroy(void) {free(reg);}

int exec(void);
int instr_parse(uint32_t);

#endif
