// Tensor VM - cpu/instr_set.c

#include "instr_set.h"
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

int operation_parse(uint16_t addr)
{
	switch (vmram[addr])
	{
		case 0x40: ;
		default: ;
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
