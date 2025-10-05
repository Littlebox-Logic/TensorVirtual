// Tensor VM - cpu/interrupt.h

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

int interrupt(uint8_t);
void int_return(void);

#endif
