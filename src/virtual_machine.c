// Tensor VM - virtual_machine.c

#include "virtual_machine.h"
#include "log.h"
#include "bios/bios.h"
#include "cpu/x86_cpu.h"
#include "vmdisk/vmfloppy.h"
#include "cpu/instr_set.h"
#include "memory/x86_mem.h"
#include "monitor/display_core.h"

#include <stdlib.h>
#include <stdio.h>

void show_instr(void)
{
	uint32_t addr = (reg->cs << 4) + reg->ip;
	printf("Instrucion \033[;32m0x\033[;92m%02X\033[0m\tat \033[;32m0x\033[;92m%05X \033[0m(CS:IP -> \033[;32m0x\033[;92m%04X\033[0m:\033[;32m0x\033[;92m%04X\033[0m)\n", vmram->ram[addr], addr, reg->cs, reg->ip);
	char instr_info[53] = {'\0'};
	sprintf(instr_info, "Instrucion 0x%02X at 0x%05X (CS:IP -> 0x%04X:0x%04X)", vmram->ram[addr], addr, reg->cs, reg->ip);
	text_output(instr_info, 255, 255, 255, true);
}

int vm_boot(void)
{
	Log(INFO, "Booting virtual machine.");
	text_output("Booting virtual machine...", 255, 255, 255, true);
	if (bios_init())	return EXIT_FAILURE;

	return 0;
}

int vm_continue(void)
{
	int exec_status;
	Log(INFO, "CPU status: continue.");
	next_instr(1);

	if ((exec_status = exec()) == 1)	return 0;
	else
	{
		Log(INFO, "Destroying virtual image: A:...");
		destroy_img(image_A, false);
		return exec_status;
	}
}
