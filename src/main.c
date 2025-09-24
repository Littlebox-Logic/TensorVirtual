// Tensor VM - main.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/utsname.h>

#include "log.h"
#include "helpinfo.c"
#include "cpu/x86_cpu.h"
#include "virtual_machine.h"
#include "memory/x86_mem.h"

bool log_enabled = true;

int main(int argc, char *argv[], char **envp)
{
	struct utsname sysinfo;
	char input[256];

	printf("Logic \033[;97mTensor VM\033[0m (Pre-alpha)\n\tBuild-0.1.0.0\n\tCoded by Logic.\n\n");

	if (argc == 1 || !strcmp(argv[1], "--help"))
	{
		printf("%s", helpinfo);
		return 0;
	}

	if (!strcmp(argv[1], "--version"))	return 0;
	if (!strcmp(argv[1], "--quiet"))	log_enabled = false;

	Log(INFO, "Tensor VM is initializing.\n");

	if (uname(&sysinfo) == 0)
	{
		Log(INFO, "OS TYPE\t: %s", sysinfo.sysname);
		Log(INFO, "OS RELEASE\t: %s", sysinfo.release);
		Log(INFO, "OS VERSION\t: %s", sysinfo.version);
		Log(INFO, "MACHINE NAME\t: %s", sysinfo.nodename);
		Log(INFO, "MACHINE TYPE\t: %s\n", sysinfo.machine);
	}

	Log(INFO, "Booting <8086 Real-Mode>.");

	if (cpu_init()) goto HALT;
	show_reg();
	if (mem_init()) goto HALT;

	Log(ERROR, "No Bootable device found.");
	Log(WARN, "No Virtual Mathine is running.");
	Log(DEBUG, "Starting DEBUG-mode.\n");

	while (1)
	{
		printf("[\033[;97;4mTensor VM \033[;97m> \033[0m");
		scanf("%[^\n]", input);
		getchar();

		if (strcmp(input, ""))	Log(DEBUG, "New instruction GET : %s", input);

		if (!strcmp(input, "exit"))
		{
			Log(DEBUG, "DEBUG-mode terminated.\n");
			break;
		}

		if (!strcmp(input, "reg"))	show_reg();
		if (!strcmp(input, "boot"))	vm_boot();
		if (!strcmp(input, "clear")) system("clear");

		memset(input, '\0', 256);
	}

HALT:
	free(reg);
	free(vmram);
	Log(INFO, "Shutdown.");

	return 0;
}
