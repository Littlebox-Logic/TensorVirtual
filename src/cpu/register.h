// Tensor VM - cpu/register.h

#ifndef REGISTER_H
#define REGISTER_H

#include <stdint.h> 

typedef struct
{
	uint16_t ax, bx, cx, dx;	// 通用寄存器
	uint16_t si, di, bp, sp;	// 指针寄存器
	uint16_t cs, ds, es, ss;	// 段寄存器
	uint16_t ip, flags;			// 指令指针, 标志寄存器
} registers, *reg_p;

#endif
