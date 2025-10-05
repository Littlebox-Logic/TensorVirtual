// Tensor VM - cpu/x86_cpu.c

#include <stdlib.h>

#include "x86_cpu.h"
#include "register.h"
#include "instr_set.h"
#include "../memory/x86_mem.h"
#include "../log.h"

reg_p reg = NULL;

int cpu_init(void)
{
	if ((reg = (reg_p)malloc(sizeof(registers))) == NULL)
	{
		Log(ERROR, "Failed to initialize Registers.");
		return EXIT_FAILURE;
	}

	cpu_reset();
	Log(INFO, "CPU initialized.");
	return 0;
}

void cpu_reset(void)
{
	reg->cs = 0xF000;
	reg->ip = 0xFFF0;
	reg->ax = reg->bx = reg->cx = reg->dx = 0x0000;
	reg->ds = reg->es = reg->ss = 0x0000;
	reg->sp = 0x0400;
	reg->flags = 0x0002;
	Log(INFO, "CPU has been RESET.");
	Log(INFO, "Stack base \033[;92mSS:SP\033[;97m -> \033[;32m0x\033[;92m0000\033[;32m:0x\033[;92m0400\033[;97m; size: \033[;92m1 KiB\033[;97m.");
}

void rom_int(void)
{
	vmram->part.rom.part.bios[0xFF00] = 0x0F;	// INT 00h
	vmram->part.rom.part.bios[0xFF01] = 0x1A;

	vmram->part.rom.part.bios[0xFF02] = 0x0F;	// INT 01h
	vmram->part.rom.part.bios[0xFF03] = 0x1B;

	vmram->part.rom.part.bios[0xFF04] = 0x0F;	// INT 02h
	vmram->part.rom.part.bios[0xFF05] = 0x1C;

	vmram->part.rom.part.bios[0xFF06] = 0x0F;	// INT 03h
	vmram->part.rom.part.bios[0xFF07] = 0x1D;

	vmram->part.rom.part.bios[0xFF08] = 0x0F;	// INT 04h
	vmram->part.rom.part.bios[0xFF09] = 0x1E;

	for (uint8_t pointer = 0; pointer <= 16; pointer += 4)
	{
		vmram->ram[pointer] = pointer / 2;
		vmram->ram[pointer + 1] = 0xFF;
		vmram->ram[pointer + 2] = 0x00;
		vmram->ram[pointer + 3] = 0xF0;
	}

	Log(INFO, "Registered ROM-Built-in interruptions 0x00 - 0x04.");
}

void show_reg(void)
{
	Log(INFO, "Reading REGISTERS data.");
	printf("\nRegister INFO:\n\
\tAX: 0x\033[;97m%04X\033[0m\tBX\t: 0x\033[;97m%04X\033[0m\tCX: 0x\033[;97m%04X\033[0m\tDX: 0x\033[;97m%04X\033[0m\n\
\tSI: 0x\033[;97m%04X\033[0m\tDI\t: 0x\033[;97m%04X\033[0m\tBP: 0x\033[;97m%04X\033[0m\tSP: 0x\033[;97m%04X\033[0m\n\
\tCS: 0x\033[;97m%04X\033[0m\tDS\t: 0x\033[;97m%04X\033[0m\tES: 0x\033[;97m%04X\033[0m\tSS: 0x\033[;97m%04X\033[0m\n\
\tIP: 0x\033[;97m%04X\033[0m\tFLAGS\t: 0x\033[;97m%04X\033[0m\n\n",	reg->ax, reg->bx, reg->cx, reg->dx,\
									reg->si, reg->di, reg->bp, reg->sp,\
									reg->cs, reg->ds,reg->es, reg->ss,\
									reg->ip, reg->flags);
}

int instr_parse(uint32_t addr)
{
	return operation_parse(addr); // ? -1 : 1;
}

int exec(void)
{
	int exec_status;
	while (!(exec_status = instr_parse((reg->cs << 4) + reg->ip)));		// Integer Promotion Rules: uint16_t -> int32_t. Need: 20-bit -> 1 MiB.

	if (exec_status == 1)	{Log(INFO, "<CPU executing terminated.>");}
	else					{Log(INFO, "<CPU executing dead - \033[;31mcode: \033[;91m%d\033[;97m>", exec_status);}

	return exec_status;
}
