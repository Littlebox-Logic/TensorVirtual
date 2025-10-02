#!/usr/bin/make

# Tensor VM - Makefile

all:				tensor

tensor:				obj/main.o	obj/bios.o	obj/x86_cpu.o	obj/x86_mem.o	obj/instr_set.o	obj/monitor.o	obj/vmfloppy.o	obj/virtual_machine.o
	cd obj && gcc main.o bios.o x86_cpu.o x86_mem.o monitor.o vmfloppy.o virtual_machine.o instr_set.o -lreadline -o ../bin/tensor && echo "Build Successfully."

obj/main.o:			src/main.c	src/log.h	src/helpinfo.c
	gcc -c -std=gnu23 -Wall src/main.c				-o obj/main.o	-lreadline

obj/bios.o:			src/bios/bios.c	src/bios/bios.h
	gcc -c -std=gnu23 -Wall src/bios/bios.c			-o obj/bios.o

obj/x86_cpu.o:		src/cpu/x86_cpu.c	src/cpu/x86_cpu.h
	gcc -c -std=gnu23 -Wall src/cpu/x86_cpu.c		-o obj/x86_cpu.o

obj/x86_mem.o:		src/memory/x86_mem.c	src/memory/x86_mem.h
	gcc -c -std=gnu23 -Wall src/memory/x86_mem.c	-o obj/x86_mem.o

obj/instr_set.o:	src/cpu/instr_set.c	src/cpu/instr_set.h
	gcc -c -std=gnu23 -Wall src/cpu/instr_set.c	-o obj/instr_set.o

obj/monitor.o:		src/monitor/monitor.c	src/monitor/monitor.h	src/monitor/display_core.c	src/monitor/display_core.h
	gcc -c -std=gnu23 -Wall src/monitor/monitor.c	-o obj/monitor.o

obj/vmfloppy.o:		src/vmdisk/vmfloppy.c	src/vmdisk/vmfloppy.h
	gcc -c -std=gnu23 -Wall src/vmdisk/vmfloppy.c	-o obj/vmfloppy.o

obj/virtual_machine.o:	src/virtual_machine.c	src/virtual_machine.h
	gcc -c -std=gnu23 -Wall src/virtual_machine.c	-o obj/virtual_machine.o

clean:
	rm -f obj/*
	rm -f bin/*

install:
	bin/tensor -
