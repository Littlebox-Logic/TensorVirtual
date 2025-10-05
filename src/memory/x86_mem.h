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
#define PART_ROM_SIZE	256

/* PC/AT Memory Mapping (PC/XT: 0xE0000 - 0xEFFFF reserved)
 *
 * 0x00000 - 0x9FFFF : RAM												640	KiB
 * 0xA0000 - 0xBFFFF : Video Memory										128	KiB
 * 0xC0000 - 0xDFFFF : Adapter-ROM (Disk controller, EGA/VGA BIOS...)	128	KiB
 * 0xE0000 - 0xEFFFF : Ex-ROM-BIOS										 64	KiB
 * 0xF0000 - 0xFFFFF : ROM-BIOS											 64	KiB
 */

typedef union
{
	uint8_t ram[MEM_SIZE];
	struct
	{
		uint8_t ram[PART_RAM_SIZE * KBYTES];
		uint8_t video[PART_VIDEO_SIZE * KBYTES];
		union
		{
			uint8_t rom[PART_ROM_SIZE * KBYTES];
			struct
			{
				uint8_t adapter[128 * KBYTES];
				uint8_t ex_bios[64 * KBYTES];
				uint8_t bios[64 * KBYTES];
			} part;
		} rom;
	} part;
} memory, *mem_p;

static_assert(sizeof(memory) == MEM_SIZE, "内存块联合体未对齐");

extern mem_p vmram;

int mem_init(void);
inline void mem_destroy(void) {free(vmram);}

#endif
