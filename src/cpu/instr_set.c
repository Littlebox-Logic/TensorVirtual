// Tensor VM - cpu/instr_set.c

#include <string.h>

#include "instr_set.h"
#include "x86_cpu.h"
#include "interrupt.h"
#include "../bios/bios_interrupt.h"
#include "../memory/x86_mem.h"
#include "../monitor/display_core.h"
#include "../virtual_machine.h"
#include "../log.h"

/* __________________________________________________________________
 * | Operation Code													|
 * __________________________________________________________________
 * | Hex		|	Instruction 		|	Description				|
 * __________________________________________________________________
 * | 40 - 47	|	INC  <Reg>			|	16-bit Reg add, 1 byte	|
 * | 48 - 4F	|	DEC  <Reg>			|	16-bit Reg sub, 1 byte	|
 * | 50 - 57	|	PUSH <Reg>			|	16-bit Reg push stack	|
 * | 90			|	NOP					|	Dummy operation			|
 * | B0 - B7	|	MOV  <Reg>, imm8	|	8 -bit imm-num -> Reg	|
 * | B8 - BF	|	MOV  <Reg>, imm16	|	16-bit imm-num -> Reg	|
 * | F4			|	HLT					|	CPU pause				|
 * | F8			|	CLC					|	Clear carry signal (CF)	|
 * | F9			|	STC					|	Set   carry signal (CF)	|
 * | FA			|	CLI					|	Clear INT allow signal	|
 * | FB			|	STI					|	Set   INT allow signal	|
 * __________________________________________________________________
 */

/* ______________________________________________________________________________
 * | Operation Code Group (First byte: 0x80 , 8-bit imm-num & mem/reg)			|
 * ______________________________________________________________________________
 * | ModR/M reg/opcode	|	Instruction	|	Description							|
 * ______________________________________________________________________________
 * | 000b	0			|	ADD			|										|
 * | 001b	1			|	OR			|										|
 * | 010b	2			|	ADC			|	Add with carry       (tgt+src+CF)	|
 * | 011b	3			|	SBB			|	Subtract with borrow (tgt-src-CF)	|
 * | 100b	4			|	AND			|										|
 * | 101b	5			|	SUB			|										|
 * | 110b	6			|	XOR			|										|
 * | 111b	7			|	CMP			|	Compare (Sub)						|
 * ______________________________________________________________________________
 */

/* ______________________________________________________________________________
 * | Operation Code Group (First byte: 0xFF)	0b11-reg-r/m -> 8-bit			|
 * ______________________________________________________________________________
 * | ModR/M reg/opcode	|	Instruction		|	Description						|
 * ______________________________________________________________________________
 * | 000b	0			|	INC  r/m16		|									|
 * | 001b	1			|	DEC  r/m16		|									|
 * | 010b	2			|	CALL r/m16		|									|
 * | 011b	3			|	CALL far m16:16	|									|
 * | 100b	4			|	JMP  r/m16		|	local jump (change reg IP only)	|
 * | 101b	5			|	JMP  far m16:16	|									|
 * | 110b	6			|	PUSH r/m16		|									|
 * ______________________________________________________________________________
 */

// Stack's physical address: "SS << 4 + SP" (push: SP-, pop: SP+)
// SS: "Stack Segment Register"
// SP: "Stack Pointer Register" (16-bit: 0x0000 -> 0xFFFF)
// Max stack segment: 64 KiB

#define SAVE_LEVEL 64
uint16_t save_count = 0;

void csip_debug(void)
{
	Log(DEBUG, "CS:IP -> \033[;32m0x\033[;92m%04X\033[;32m:0x\033[;92m%04X\033[;97m -> \033[;32m0x\033[;92m%05X.\033[;97m -> INST \033[;32m0x\033[;92m%02X\033[;97m", reg->cs, reg->ip, (reg->cs << 4) + reg->ip, vmram->ram[(reg->cs << 4) + reg->ip]);
}

void next_instr(uint8_t instr_length)
{
	if (reg->ip + instr_length <= 0xFFFF)	reg->ip += instr_length;

	else
	{
		reg->cs += 0x1000;
		reg->ip -= (0xFFFF - instr_length + 1);
	}

	csip_debug();
}

int operation_parse(uint32_t addr)
{
	uint8_t instr_length = 1;			// Default: single-byte operate code.
	uint16_t *reg_table[8]	= {&(reg->ax), &(reg->cx), &(reg->dx), &(reg->bx), &(reg->sp), &(reg->bp), &(reg->si), &(reg->di)};
	uint16_t *sreg_table[4]	= {&(reg->es), &(reg->cs), &(reg->ss), &(reg->ds)};

	switch (vmram->ram[addr])			// Attention: ``Case Range'' ONLY for ``GNU C''.
	{
		case 0x0F:						// Reserved INT & Others.
			instr_length = 0;
			switch (vmram->ram[addr + 1])
			{
				case 0x1A: rom_int_0();		break;
				case 0x1B: rom_int_1();		return 1;
				case 0x1C: rom_int_2();		break;
				case 0x1D: rom_int_3();		break;
				case 0x1E: rom_int_4();		break;
				case 0x2A: bios_int_10();	break;
				default  : instr_length = 2;
			}
			break;

		case 0x40 ... 0x47:				// INC <Reg> (single-byte operate code).
			(*reg_table[vmram->ram[addr] - 0x40])++;
			break;

		case 0x48 ... 0x4F:				// DEC <Reg> (single-byte operate code).
			(*reg_table[vmram->ram[addr] - 0x40])--;
			break;

		case 0x50 ... 0x53:				// PUSH <Reg> (single-byte operate code).
		case 0x55 ... 0x57:
			reg->sp -= 2;
			memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), reg_table[vmram->ram[addr] - 0x50], 2);
			break;
		case 0x54:
			memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->sp, 2); // Special operation order.
			reg->sp -= 2;
			break;

		case 0x88 ... 0x8B:				// MOV r16, r16	(0b 100010dw 11rrrmmm)	*
			;
			break;

		case 0x8C:						// 8C /r : 8C mod reg r/m. MOV for segment registers.
			instr_length = 2;
			if (vmram->ram[addr + 1] >> 6 == 0b11)	// Register mode	: MOV r16, sreg
				*reg_table[vmram->ram[addr + 1] & 0b00000111] = *sreg_table[vmram->ram[addr + 1] >> 3 & 0b00000111];
			else									// Memory mode		: MOV mem16, sreg
			{
				;									// *** developing.
			}
			break;
		case 0x8E:						// MOV DS, [...]	or 8E /r: mov sreg, rrr	*
			instr_length = 2;
			if (vmram->ram[addr + 1] >> 6 == 0b11)	// Register mode	: MOV sreg, r16
				*sreg_table[vmram->ram[addr + 1] >> 3 & 0b00000111] = *reg_table[vmram->ram[addr + 1] & 0b00000111]; 
			else									// Memory mode		: MOV sreg, mem16
			{
				;									// *** developing.
			}
			break;

		case 0x90: break;				// NOP (single-byte operate code).

		case 0xB8 ... 0xBF:				// MOV r16, imm16
			instr_length = 3;
			*reg_table[vmram->ram[addr] - 0xB8] = vmram->ram[addr + 1] + (vmram->ram[addr + 2] << 8);
			break;

		case 0xC3: break;				// RET (single-byte operate code).	*
		case 0xCB: break;				// RETF (single-byte operate code).	*

		case 0xCC:						// INT 3 (single-byte operate code).
			instr_length = 0;
			interrupt(3);
			break;
		case 0xCD:						// INT n (n != 3)
			instr_length = 0;
			interrupt(vmram->ram[addr + 1]);
			break;
		case 0xCF:						// IRET (single-byte operate code) -> INT RETURN.
			instr_length = 0;
			int_return();
			break;

		case 0xF4:						// CPU Pause.
			Log(DEBUG, "Detected \033[;32m0x\033[;92mF4\033[;97m at \033[;32m0x\033[;92m%05X\033[;97m (\033[;32m0x\033[;92m%04X\033[;97m:\033[;32m0x\033[;92m%04X\033[;97m) -> CPU pause.", addr, reg->cs, reg->ip);
			return 1;

		case 0xFF:
			instr_length = 2;
			switch (vmram->ram[addr + 1] >> 3)
			{
				case 0b11000: break;	// INC  r/m16
				case 0b11001: break;	// DEC  r/m16
				case 0b11010: break;	// CALL r/m16
				case 0b11011: break;	// CALL far m16:16
				case 0b11100:			// JMP  r/m16
					reg->ip = *reg_table[vmram->ram[addr + 1] & 0b00000111];
					instr_length = 0;
					break;
				case 0b11101: break;	// JMP  far m16:16
				case 0b11110: break;	// PUSH r/m16
			}

			break;
	}

	next_instr(instr_length);
	if (reg->flags & 0x0100)	return 1;

	save_count++;
	if (save_count >= SAVE_LEVEL)
	{
		text_output("Save Mode Enabled: paused.", 255, 0, 0, true);
		show_reg(true);
		show_instr();
		return 1;
	}

	return 0;
}

inline void add_reg_reg(uint8_t tgt_reg, uint8_t src_reg)
{
	;
}

inline void mov_reg_reg(uint8_t tgt_reg, uint8_t src_reg)
{
	;
}

inline void mov_reg_data(uint8_t tgt_reg, int16_t src_data)
{
	;
}
