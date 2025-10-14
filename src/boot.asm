org 0x7c00

mov ax, 0x001d
jmp ax
db '<INT 10h interrupt test>'

mov ax, 0x07c0
mov es, ax
mov bp, 0x0005
mov cx, 24
mov ax, 0x1300
int 10h

inc ax
inc bx
inc cx
inc ax

nop
db 255
nop
times 5 inc ax
times 5 dec ax

mov dx, 0xABCD
int 0
int 0
int 1

hlt

times 510 - ($ - $$) db 0
dw 0xaa55
