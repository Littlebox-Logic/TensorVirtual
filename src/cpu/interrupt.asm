; Tensor VM - cpu/interrupt.asm

; int 00h
; 0x0000 -> IP = 0xFF23
; 0x0002 -> CS = 0xF000

org 0xFF23
divide_error:
	push	ax
	mov		ax, 0xB800
	mov		es, ax
	mov		byte	[es:0], 'D'
	mov		byte	[es:2], 'I'
	mov		byte	[es:4], 'V'
	pop		ax
	iret

	times 0xFF4E - ($ - divide_error)	db 0

; int 01h
; 0x0004 -> IP = 0xFF4E
; 0x0006 -> CS = 0xF000

org 0xFF4E
single_step:
	iret

	times 0xFF5F - ($ - single_step)	db 0

; int 02h
; 0x0008 -> IP = 0xFF5F
; 0x000A -> CS = 0xF000

org  0xFF5F
nmi:
	push	ax

	; Test parity error.
	in		al, 0x61		; Read system control port.
	test	al. 0x40		; Check parity error bit.
	jz		.not_parity_error

	; Handle memory parity error.

