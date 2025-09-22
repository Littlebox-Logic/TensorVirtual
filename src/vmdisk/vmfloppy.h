// Tensor VM - vmdisk/vmfloppy.h

#ifndef VM_FLOPPY_H
#define VM_FLOPPY_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// 2880 扇区 * 512 字节/扇区 = 1474560 字节.

typedef struct
{
	FILE *img_record;		// 8 bits.
	char *img_path;			// 8 bits. 
	size_t img_size;		// 8 bits
	char description[6];	// 6 bits.
	bool write_protected;	// 1 bit.
	uint8_t drive_id;		// 1 bit.
} floppy, *vmfloppy;

vmfloppy insert_flp(char *, uint8_t);
int load_floppy(vmfloppy);
void destroy_img(vmfloppy, bool);

#endif
