org 0x7c00

dw 182
dw 23
jmp 0x7c00
nop
db 255
nop
times 20 db 72

times 510 - ($ - $$) db 0
dw 0xaa55
