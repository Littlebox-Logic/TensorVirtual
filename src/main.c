// Tensor VM - main.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <pthread.h>

#ifdef _WIN32
#include <windows.h>
#undef ERROR

#else
#include <sys/utsname.h>
#include <readline/readline.h>
#include <readline/history.h>
// #include <unistd.h>

#endif

#include "log.h"
#include "helpinfo.c"
#include "cpu/x86_cpu.h"
#include "virtual_machine.h"
#include "memory/x86_mem.h"
#include "monitor/monitor.h"
#include "monitor/display_core.h"

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

	// pthread_t monitor_sdl_thread;

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

	if (cpu_init())		goto HALT;
	show_reg(false);
	if (mem_init())		goto HALT;
	rom_int();
	if (monitor_init())	goto HALT;

	//pthread_create(&monitor_sdl_thread, NULL, monitor_thread, NULL); 
	Log(INFO, "Monitor status: \033[;92mPower-On\033[;97m.");

	Log(ERROR, "No Bootable device found.");
	Log(WARN, "No Virtual Mathine is running.");
	Log(DEBUG, "Starting DEBUG-mode.\n");

	printf("Type \"\033[;97mhelp\033[0m\" to get HELP info.\n");
	while (true)
	{
		// if (sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP)
		/*if (sdl_event.type == SDL_KEYDOWN && sdl_event.key.keysym.sym == SDLK_ESCAPE)
		{
			Log(INFO, "<Esc>");
			break;
		}*/
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

		if (!strcmp(input, "reg"))		show_reg(true);
		if (!strcmp(input, "boot"))		{clear_screen(true); hello(); vm_boot();}
		if (!strcmp(input, "play"))
		{
			clear_screen(true);
			text_output("Line 1.", 255, 255, 255, 1);
			text_output("Line 2.", 0, 255, 0, 0);
			text_output("Line 2.next.", 0, 0, 255, 0);
			for (int i = 0; i < 80; i++)
			{
				text_output("Line new.", 255, 255, 255, true);
			}
		}
		if (!strcmp(input, "c"))		vm_continue();
		if (!strcmp(input, "instr"))	show_instr();
		if (!strcmp(input, "clear"))	printf("\033[2J\033[H"); // like system("clear");
		if (!strcmp(input, "help"))		show_help();

		#ifdef _WIN32
		memset(input, '\0', 256);	// When no ``readline''
		#endif
	}

	monitor_on = false;
	//pthread_join(monitor_sdl_thread, NULL);
	text_free();

HALT:
	free(reg);
	free(vmram);
	monitor_destroy();
	Log(INFO, "Shutdown.");

	return 0;
}
