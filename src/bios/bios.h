// Tensor VM - bios/bios.h

#ifndef BIOS_H
#define BIOS_H

#include <stdint.h>

#define INSTR_BASE	0xFFFF0
#define CS_INIT		0xF000
#define IP_INIT		0xFFF0

#define MBR_BASE	0x7C00

int bios_init(void);
void show_mbr_sector(uint8_t *);

#endif
