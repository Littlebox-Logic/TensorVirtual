// Tensor VM - memory/x86_mem.h

#ifndef X86_MEM_H
#define X86_MEM_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define MEM_SIZE	1048576
#define BOOT_START	0x7C00
#define KBYTES		1024

#define PART_RAM_SIZE	640
#define PART_VIDEO_SIZE	128
#define PART_BIOS_SIZE	256

typedef union
{
	uint8_t ram[MEM_SIZE];
	struct
	{
		uint8_t ram[PART_RAM_SIZE * KBYTES];
		uint8_t video[PART_VIDEO_SIZE * KBYTES];
		uint8_t bios[PART_BIOS_SIZE * KBYTES];
	} part;
} memory, *mem_p;

static_assert(sizeof(memory) == MEM_SIZE, "内存块联合体未对齐");

extern mem_p vmram;

int mem_init(void);
inline void mem_destroy(void) {free(vmram);}

#endif
