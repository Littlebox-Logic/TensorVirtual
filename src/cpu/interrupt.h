// Tensor VM - cpu/interrupt.h

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

int interrupt(uint8_t);
void int_return(void);

void rom_int_0(void);
void rom_int_1(void);
void rom_int_2(void);
void rom_int_3(void);
void rom_int_4(void);

#endif
