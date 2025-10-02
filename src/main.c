// Tensor VM - main.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>

#else
#include <sys/utsname.h>
#include <readline/readline.h>
#include <readline/history.h>

#endif

#include "log.h"
#include "helpinfo.c"
#include "cpu/x86_cpu.h"
#include "virtual_machine.h"
#include "memory/x86_mem.h"

bool log_enabled = true;

int main(int argc, char *argv[], char **envp)
{
	#ifdef _WIN32
	char input[256];				// When no ``readline''

	#else
	struct utsname sysinfo;
	char *input;
	using_history();
	#endif

	printf("Logic \033[;97mTensor VM\033[0m (Pre-alpha)\n\tBuild-0.1.0.0\n\tCoded by Logic.\n\n");

	if (argc == 1 || !strcmp(argv[1], "--help"))
	{
		printf("%s", helpinfo);
		return 0;
	}

	if (!strcmp(argv[1], "--version"))	return 0;
	if (!strcmp(argv[1], "--quiet"))	log_enabled = false;

	Log(INFO, "Tensor VM is initializing.\n");

	#ifdef _WIN32
	OSVERSIONINFO osvi;
	SYSTEM_INFO ossi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	GetSystemInfo(&ossi);

	Log(INFO, "OS TYPE\t: Windows");
	Log(INFO, "OS RELEASE\t: %ld.%ld", osvi.dwMajorVersion, osvi.dwMinorVersion);
	Log(INFO, "OS VERSION\t: %ld", osvi.dwBuildNumber);
	switch (ossi.wProcessorArchitecture)
	{
		case PROCESSOR_ARCHITECTURE_AMD64:	Log(INFO, "MATHINE TYPE\t: x86_64");	break;
		case PROCESSOR_ARCHITECTURE_INTEL:	Log(INFO, "MATHINE TYPE\t: i686");		break;
		default:							Log(INFO, "MATHINE TYPE\t: Unknown");	break;
	}
	#else
	if (uname(&sysinfo) == 0)
	{
		Log(INFO, "OS TYPE\t: %s", sysinfo.sysname);
		Log(INFO, "OS RELEASE\t: %s", sysinfo.release);
		Log(INFO, "OS VERSION\t: %s", sysinfo.version);
		Log(INFO, "MACHINE NAME\t: %s", sysinfo.nodename);
		Log(INFO, "MACHINE TYPE\t: %s\n", sysinfo.machine);
	}
	#endif

	Log(INFO, "Booting <8086 Real-Mode>.");

	if (cpu_init()) goto HALT;
	show_reg();
	if (mem_init()) goto HALT;

	Log(ERROR, "No Bootable device found.");
	Log(WARN, "No Virtual Mathine is running.");
	Log(DEBUG, "Starting DEBUG-mode.\n");

	while (1)
	{
		#ifdef _WIN32
		printf("[\033[;97;4mTensor VM \033[;97m> \033[0m");
		scanf("%[^\n]", input);		// When no ``readline''
		getchar();
		#else
		input = readline("[\033[;97;4mTensor VM \033[;97m> \033[0m");
		if (*input)	add_history(input);
		#endif

		if (strcmp(input, ""))	Log(DEBUG, "New instruction GET : %s", input);

		if (!strcmp(input, "exit"))
		{
			Log(DEBUG, "DEBUG-mode terminated.\n");
			break;
		}

		if (!strcmp(input, "reg"))		show_reg();
		if (!strcmp(input, "boot"))		vm_boot();
		if (!strcmp(input, "c"))		vm_continue();
		if (!strcmp(input, "instr"))	show_instr();
		if (!strcmp(input, "clear"))	system("clear");

		#ifdef _WIN32
		memset(input, '\0', 256);	// When no ``readline''
		#endif
	}

HALT:
	free(reg);
	free(vmram);
	Log(INFO, "Shutdown.");

	return 0;
}
