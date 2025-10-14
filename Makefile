#!/usr/bin/make

# Tensor VM - Makefile

all:				tensor	tensor.exe

tensor:				obj/main.o	obj/bios.o	obj/bios_interrupt.o	obj/x86_cpu.o	obj/x86_mem.o	obj/interrupt.o	obj/instr_set.o	obj/monitor.o	obj/display_core.o	obj/vmfloppy.o	obj/virtual_machine.o	lib/libSDL3.a	lib/libSDL3_ttf.a
	cd obj && gcc main.o bios.o bios_interrupt.o x86_cpu.o x86_mem.o interrupt.o monitor.o display_core.o vmfloppy.o virtual_machine.o instr_set.o -lm -lreadline -ldrm -lfreetype -lharfbuzz -lplutosvg ../lib/libSDL3.a ../lib/libSDL3_ttf.a -o ../bin/tensor && printf "\033[;92mLinux Build Successfully.\033[0m\n"

obj/main.o:			src/main.c	src/log.h	src/helpinfo.c
	gcc -c -std=gnu23 -Wall src/main.c					-o obj/main.o

obj/bios.o:			src/bios/bios.c	src/bios/bios.h
	gcc -c -std=gnu23 -Wall src/bios/bios.c				-o obj/bios.o

obj/bios_interrupt.o:	src/bios/bios_interrupt.c	src/bios/bios_interrupt.h
	gcc -c -std=gnu23 -Wall src/bios/bios_interrupt.c	-o obj/bios_interrupt.o

obj/x86_cpu.o:		src/cpu/x86_cpu.c	src/cpu/x86_cpu.h
	gcc -c -std=gnu23 -Wall src/cpu/x86_cpu.c			-o obj/x86_cpu.o

obj/x86_mem.o:		src/memory/x86_mem.c	src/memory/x86_mem.h
	gcc -c -std=gnu23 -Wall src/memory/x86_mem.c		-o obj/x86_mem.o

obj/interrupt.o:	src/cpu/interrupt.c		src/cpu/interrupt.h
	gcc -c -std=gnu23 -Wall src/cpu/interrupt.c			-o obj/interrupt.o

obj/instr_set.o:	src/cpu/instr_set.c	src/cpu/instr_set.h
	gcc -c -std=gnu23 -Wall src/cpu/instr_set.c			-o obj/instr_set.o

obj/monitor.o:		src/monitor/monitor.c	src/monitor/monitor.h
	gcc -c -std=gnu23 -Wall src/monitor/monitor.c		-o obj/monitor.o

obj/display_core.o:	src/monitor/display_core.c	src/monitor/display_core.h
	gcc -c -std=gnu23 -Wall src/monitor/display_core.c	-o obj/display_core.o

obj/vmfloppy.o:		src/vmdisk/vmfloppy.c	src/vmdisk/vmfloppy.h
	gcc -c -std=gnu23 -Wall src/vmdisk/vmfloppy.c		-o obj/vmfloppy.o

obj/virtual_machine.o:	src/virtual_machine.c	src/virtual_machine.h
	gcc -c -std=gnu23 -Wall src/virtual_machine.c		-o obj/virtual_machine.o

tensor.exe:				obj/main_win.o	obj/bios_win.o	obj/bios_interrupt_win.o	obj/x86_cpu_win.o	obj/x86_mem_win.o	obj/interrupt_win.o	obj/instr_set_win.o	obj/monitor_win.o	obj/display_core_win.o	obj/vmfloppy_win.o	obj/virtual_machine_win.o
	cd obj && x86_64-w64-mingw32-gcc main_win.o bios_win.o bios_interrupt_win.o x86_cpu_win.o x86_mem_win.o interrupt_win.o monitor_win.o display_core_win.o vmfloppy_win.o virtual_machine_win.o instr_set_win.o -lSDL3.dll -lSDL3_ttf.dll -static -o ../bin/tensor.exe && printf "\033[;92mWindows Build Successfully.\033[0m\n"

obj/main_win.o:			src/main.c	src/log.h	src/helpinfo.c
	x86_64-w64-mingw32-gcc -c -std=gnu23 -Wall src/main.c					-o obj/main_win.o

obj/bios_win.o:			src/bios/bios.c	src/bios/bios.h
	x86_64-w64-mingw32-gcc -c -std=gnu23 -Wall src/bios/bios.c				-o obj/bios_win.o

obj/bios_interrupt_win.o:	src/bios/bios_interrupt.c	src/bios/bios_interrupt.h
	x86_64-w64-mingw32-gcc -c -std=gnu23 -Wall src/bios/bios_interrupt.c	-o obj/bios_interrupt_win.o

obj/x86_cpu_win.o:		src/cpu/x86_cpu.c	src/cpu/x86_cpu.h
	x86_64-w64-mingw32-gcc -c -std=gnu23 -Wall src/cpu/x86_cpu.c			-o obj/x86_cpu_win.o

obj/x86_mem_win.o:		src/memory/x86_mem.c	src/memory/x86_mem.h
	x86_64-w64-mingw32-gcc -c -std=gnu23 -Wall src/memory/x86_mem.c			-o obj/x86_mem_win.o

obj/interrupt_win.o:	src/cpu/interrupt.c		src/cpu/interrupt.h
	x86_64-w64-mingw32-gcc -c -std=gnu23 -Wall src/cpu/interrupt.c			-o obj/interrupt_win.o

obj/instr_set_win.o:	src/cpu/instr_set.c	src/cpu/instr_set.h
	x86_64-w64-mingw32-gcc -c -std=gnu23 -Wall src/cpu/instr_set.c			-o obj/instr_set_win.o

obj/monitor_win.o:		src/monitor/monitor.c	src/monitor/monitor.h
	x86_64-w64-mingw32-gcc -c -std=gnu23 -Wall src/monitor/monitor.c		-o obj/monitor_win.o

obj/display_core_win.o:		src/monitor/display_core.c	src/monitor/display_core.h
	x86_64-w64-mingw32-gcc -c -std=gnu23 -Wall src/monitor/display_core.c	-o obj/display_core_win.o

obj/vmfloppy_win.o:		src/vmdisk/vmfloppy.c	src/vmdisk/vmfloppy.h
	x86_64-w64-mingw32-gcc -c -std=gnu23 -Wall src/vmdisk/vmfloppy.c		-o obj/vmfloppy_win.o

obj/virtual_machine_win.o:	src/virtual_machine.c	src/virtual_machine.h
	x86_64-w64-mingw32-gcc -c -std=gnu23 -Wall src/virtual_machine.c		-o obj/virtual_machine_win.o

clean:
	rm -f obj/*
	rm -f bin/*

install:
	bin/tensor -
