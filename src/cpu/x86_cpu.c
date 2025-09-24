// Tensor VM - cpu/x86_cpu.c

#include <stdlib.h>

#include "x86_cpu.h"
#include "register.h"
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
	reg->flags = 0x0002;
	Log(INFO, "CPU has been RESET.");
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

inline int instr_parse(uint16_t addr)
{
	;
}

void exec(void)
{
	while (!(exec_status = instr_parse(reg->cs << 4 + reg_ip)));

	if (exec status == 1)	Log(INFO, "<CPU executing terminated.>");
	else					Log(INFO, "<CPU executing dead - \033[;31mcode: \033[;91m%d\033[;97m>", exec_status);
}
