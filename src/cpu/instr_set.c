// Tensor VM - cpu/instr_set.c

#include <string.h>

#include "instr_set.h"
#include "x86_cpu.h"
#include "../memory/x86_mem.h"
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
 * | 000b				|	ADD			|										|
 * | 001b				|	OR			|										|
 * | 010b				|	ADC			|	Add with carry       (tgt+src+CF)	|
 * | 011b				|	SBB			|	Subtract with borrow (tgt-src-CF)	|
 * | 100b				|	AND			|										|
 * | 101b				|	SUB			|										|
 * | 110b				|	XOR			|										|
 * | 111b				|	CMP			|	Compare (Sub)						|
 * ______________________________________________________________________________
 */

/* ______________________________________________________________________________
 * | Operation Code Group (First byte: 0xFF)									|
 * ______________________________________________________________________________
 * | ModR/M reg/opcode	|	Instruction	|	Description							|
 * ______________________________________________________________________________
 * |
 *
 *
 */

// Stack's physical address: "SS << 4 + SP" (push: SP-, pop: SP+)
// SS: "Stack Segment Register"
// SP: "Stack Pointer Register" (16-bit: 0x0000 -> 0xFFFF)
// Max stack segment: 64 KiB

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
	uint8_t instr_length = 1;			// Default: single-bype operate code.

	switch (vmram->ram[addr])
	{
		case 0x40: reg->ax++; break;	// INC <Reg> single-byte operate code.
		case 0x41: reg->cx++; break;
		case 0x42: reg->dx++; break;
		case 0x43: reg->bx++; break;
		case 0x44: reg->sp++; break;
		case 0x45: reg->bp++; break;
		case 0x46: reg->si++; break;
		case 0x47: reg->di++; break;

		case 0x48: reg->ax--; break;	// DEC <Reg> single-byte operate code.
		case 0x49: reg->cx--; break;
		case 0x4A: reg->dx--; break;
		case 0x4B: reg->bx--; break;
		case 0x4C: reg->sp--; break;
		case 0x4D: reg->bp--; break;
		case 0x4E: reg->si--; break;
		case 0x4F: reg->di--; break;

		case 0x50:						// PUSH <Reg> single-byte operate code.
			reg->sp -= 2;
			memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->ax, 2);
			break;
		case 0x51:
			reg->sp -= 2;
			memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->cx, 2);
			break;
		case 0x52:
			reg->sp -= 2;
			memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->dx, 2);
			break;
		case 0x53:
			reg->sp -= 2;
			memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->bx, 2);
			break;
		case 0x54:
			memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->sp, 2); // Special operation order.
			reg->sp -= 2;
			break;
		case 0x55:
			reg->sp -= 2;
			memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->bp, 2);
			break;
		case 0x56:
			reg->sp -= 2;
			memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->si, 2);
			break;
		case 0x57:
			reg->sp -= 2;
			memcpy(&(vmram->ram[(reg->ss << 4) + reg->sp]), &reg->di, 2);
			break;

		case 0x90: break;				// NOP single-byte operate code.

		case 0xF4:						// CPU Pause.
			Log(DEBUG, "Detected \033[;32m0x\033[;92mF4\033[;97m at \033[;32m0x\033[;92m%05X\033[;97m (code: \033[;32m0x\033[;92m%02X\033[;97m) -> CPU pause.", addr, vmram->ram[addr]);
			return 1;
	}

	next_instr(instr_length);
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
