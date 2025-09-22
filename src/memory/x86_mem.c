// Tensor VM - memory/x86_mem.c

#include <stdlib.h>
#include <string.h>

#include "x86_mem.h"
#include "../log.h"

mem_p vmram = NULL;

int mem_init(void)
{
	if ((vmram = (mem_p)malloc(sizeof(memory))) != NULL)
	{
		Log(INFO, "Initialized VM Memory; size : 1 MiB.");
		return 0;
	}
	else
	{
		Log(ERROR, "Failed to initialize VM Memory.");
		return EXIT_FAILURE;
	}
}
