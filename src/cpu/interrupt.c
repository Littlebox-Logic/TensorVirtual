// Tensor VM - cpu/interrupt.c

#include <string.h>

#include "interrupt.h"
#include "x86_cpu.h"
#include "../memory/x86_mem.h"
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

	reg->sp -= 4;
	memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->cs, 2);		// PUSH CS
	memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->ip, 2);		// PUSH IP

	reg->flags &= 0b11111110011111111;

	reg->ip = (vmram->ram[ivn * 4 + 1] << 8) + vmram->ram[ivn * 4];
	reg->cs = (vmram->ram[ivn * 4 + 3] << 8) + vmram->ram[ivn * 4 + 2];

	return 0;
}
