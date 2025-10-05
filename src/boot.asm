org 0x7c00

inc ax
inc bx
inc cx
inc ax

jmp 0x7c00
nop
db 255
nop
times 34 inc ax
times 20 dec ax

mov dx, 0xABCD
int 10

;jmp ax

hlt

times 510 - ($ - $$) db 0
dw 0xaa55
