// Tensor VM - cpu/interrupt.c

#include <string.h>

#include "interrupt.h"
#include "x86_cpu.h"
#include "../memory/x86_mem.h"
#include "instr_set.h"
#include "../virtual_machine.h"
#include "../log.h"

/* ______________________________________________________________________
 * | FLAGS Register														|
 * ______________________________________________________________________
 * | Bit	|	Name	|	NAME					|	Note			|
 * ______________________________________________________________________
 * | 0		|	CF		|	Carry Flag				|					|
 * | 1		| (Reserved)|							|	Always 1		|
 * | 2		|	PF		|	Parity Flag				|					|
 * | 3		| (Reserved)|							|	Always 0		|
 * | 4		|	AF		|	Auxiliary Carry Flag	|					|
 * | 5		| (Reserved)|							|	Always 0		|
 * | 6		|	ZF		|	Zero Flag				|					|
 * | 7		|	SF		|	Sign Flag				|					|
 * | 8		|	TF		|	Trap Flag				|	Single-step exec| * -> 0
 * | 9		|	IF		|	Interrupt Enable Flag	|					| * -> 0
 * | 10		|	DF		|	Direction Flag			|					|
 * | 11		|	OF		|	Overflow Flag			|					|
 * | 11 - 15| (Reserved)|							|	IOPL (Undefined)|
 * ______________________________________________________________________
 */

// Interrupt Vector Table (IVT) at 0x0000:0x0000 -> 0x0000:0x03FF (1 KiB)
// Entrance address = Interrupt Vector Number (IVN) * 4

int interrupt(uint8_t ivn)
{
	uint8_t instr_length = ivn == 3 ? 1 : 2;
	reg->sp -= 2;														// PUSH FLAGS
	memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->flags, 2);

	if (reg->ip + instr_length <= 0xFFFF)	reg->ip += instr_length;

	else
	{
		reg->cs += 0x1000;
		reg->ip -= (0xFFFF - instr_length + 1);
	}

	reg->sp -= 2;
	memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->cs, 2);		// PUSH CS

	reg->sp -= 2;
	memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->ip, 2);		// PUSH IP

	reg->flags &= 0b11111110011111111;

	reg->ip = (vmram->ram[ivn * 4 + 1] << 8) + vmram->ram[ivn * 4];
	reg->cs = (vmram->ram[ivn * 4 + 3] << 8) + vmram->ram[ivn * 4 + 2];
	Log(INFO, "Starting interrupt: INT 0x%02X", ivn);
	csip_debug();
	// getchar();

	return 0;
}

void int_return(void)
{
	reg->ip = ((vmram->ram[(reg->ss << 4) + reg->sp + 1]) << 8) + (vmram->ram[(reg->ss << 4) + reg->sp]); 		// POP IP
	reg->sp += 2;
	reg->cs = ((vmram->ram[(reg->ss << 4) + reg->sp + 1]) << 8) + (vmram->ram[(reg->ss << 4) + reg->sp]);		// POP CS

	reg->sp += 2;
	reg->flags = ((vmram->ram[(reg->ss << 4) + reg->sp + 1]) << 8) + (vmram->ram[(reg->ss << 4) + reg->sp]);	// POP FLAGS
	reg->sp += 2;
}

void rom_int_0(void)	// Divide Error
{
	printf("\033[;91mINT 00h: Divide Error!\033[0m\n");
	int_return();
}

void rom_int_1(void)	// Single-step Debug
{
	show_reg();
	show_instr();
	int_return();
}

void rom_int_2(void)	// NMI		*** Developing.
{
	printf("\033[;91mINT 02h: NMI Error!\033[0m\n");
	int_return();
}

void rom_int_3(void)	// Debug
{
	show_reg();
	show_instr();
	int_return();
}

void rom_int_4(void)
{
	printf("\033[;91mINT 04h: Arithmetic Overflow!\033[0m\n");
	int_return();
}
