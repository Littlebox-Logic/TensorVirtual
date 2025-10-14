// Tensor VM - bios/bios.c

#include "bios.h"
#include "../log.h"
#include "../memory/x86_mem.h"
#include "../cpu/x86_cpu.h"

#include <stdlib.h>
#include <string.h>

/* BIOS 启动流程
 * 
 * 1) CPU 复位, 从 0xFFFF0 (CS = 0xF000, IP = 0xFFFF0) 开始执行;
 * 2) POST (上电自检, Power-On Self-Test), 可略去;
 * 3) 初始化硬件设备;
 * 4) 搜索可引导设备;
 * 5) 加载 MBR 到 0x7C00, 跳转执行.
 */

vmfloppy image_A;

void show_mbr_sector(uint8_t *boot_sector)
{
	for (uint16_t index = 0; index < 512; index++)
	{
		if (index % 16 == 0) printf("\n\t\033[0m%03X: ", index);
		printf(boot_sector[index] ? "\033[;92m%02X " : "\033[;97m%02X ", boot_sector[index]);
	}

	printf("\n\033[0m");
}

void bios_rom_int(void)
{
	for (uint8_t pointer = 0x08; pointer <= 0x1F; pointer++)
	{
		vmram->part.rom.part.bios[0xFF00 + pointer * 2]		= 0x0F;		// INT.
		vmram->part.rom.part.bios[0xFF00 + pointer * 2 + 1]	= 0x1A + pointer;
		vmram->ram[pointer * 4] = pointer * 2;
		vmram->ram[pointer * 4 + 1] = 0xFF;
		vmram->ram[pointer * 4 + 2] = 0x00;
		vmram->ram[pointer * 4 + 3] = 0xF0;
	}

	vmram->part.rom.part.bios[0xFF00 + 0x41 * 2]	 = 0x0F;		// INT.
	vmram->part.rom.part.bios[0xFF00 + 0x41 * 2 + 1] = 0x1A + 0x41;
	vmram->ram[0x41 * 4] = 0x41 * 2;
	vmram->ram[0x41 * 4 + 1] = 0xFF;
	vmram->ram[0x41 * 4 + 2] = 0x00;
	vmram->ram[0x41 * 4 + 3] = 0xF0;

	vmram->part.rom.part.bios[0xFF00 + 0x46 * 2]	 = 0x0F;		// INT.
	vmram->part.rom.part.bios[0xFF00 + 0x46 * 2 + 1] = 0x1A + 0x46;
	vmram->ram[0x46 * 4] = 0x46 * 2;
	vmram->ram[0x46 * 4 + 1] = 0xFF;
	vmram->ram[0x46 * 4 + 2] = 0x00;
	vmram->ram[0x46 * 4 + 3] = 0xF0;
}

int bios_init(void)
{
	Log(INFO, "Initialing BIOS.");
	Log(DEBUG, "Skipped Power-On Self-Test (POST).");
	bios_rom_int();
	// 初始化硬件设备暂时略去.
	// 暂定引导设备为 boot.img (1.44 MiB)
	if ((image_A = insert_flp("boot.img", 'A')) == NULL)	goto FAIL;
	if (load_floppy(image_A))
	{
		destroy_img(image_A, true);
		goto FAIL;
	}

	uint8_t boot_sector[512] = {0};
	fread(boot_sector, 1, 512, image_A->img_record);

	if (boot_sector[510] == 0x55 && boot_sector[511] == 0xaa)
	{
		Log(INFO, "Detected Boot Sector Flag: 0x55AA");
		printf("  MBR Sector:");
		show_mbr_sector(boot_sector);
	}
	else
	{
		Log(ERROR, "No Boot Sector Flag Found. Shutdown.");
		destroy_img(image_A, false);
		return EXIT_FAILURE;
	}

	memcpy(&(vmram->ram[0x7c00]), boot_sector, 512);
	Log(INFO, "Loaded MBR Sector to VMRAM. at 0x\033[;97m7c00\033[0m.");

	reg->cs = 0x07c0;
	reg->ip = 0x0000;
	Log(INFO, "Set Registers \033[;92mCS:IP\033[;97m -> \033[;32m0x\033[;92m07C0\033[;32m:0x\033[;92m0000\033[;97m -> \033[;32m0x\033[;92m7C00\033[;97m.");

	Log(INFO, "Calling CPU to execute MBR instructions...");
	if (exec() == 1)	return 0;	// Status code: 1 -> CPU Pause.

	Log(INFO, "Distroying virtual image: A:...");
	destroy_img(image_A, false);
	return 0;

FAIL:
	Log(ERROR, "Failed to initialize BIOS: No-Floppy.");
	return EXIT_FAILURE;
}
