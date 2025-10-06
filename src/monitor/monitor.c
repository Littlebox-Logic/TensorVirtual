// Tensor VM - monitor/monitor.c

#include "monitor.h"
#include "display_core.c"

#include <SDL2/SDL.h>

int monitor_init(void)
{
	#ifdef __linux__
	setenv("SDL_VIDEODRIVER", "kmsdrm", 1);
	#endif

	return 0;
}
