// Tensor VM - virtual_machine.c

#include "virtual_machine.h"
#include "log.h"
#include "bios/bios.h"

#include <stdlib.h>

int vm_boot(void)
{
	Log(INFO, "Booting virtual machine.");
	if (bios_init())	return EXIT_FAILURE;

	
	return 0;
}
