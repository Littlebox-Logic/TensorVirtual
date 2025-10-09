// Tensor VM - monitor/monitor.c

#include "monitor.h"
#include "display_core.c"
#include "../log.h"

#include <SDL3/SDL.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Rect display_bounds; // = NULL;
SDL_Event sdl_event; // = NULL;
SDL_DisplayMode *display_mode = NULL;
// SDL_Environment *env = NULL;

bool monitor_on = true;

int monitor_init(void)
{
	/* #ifdef __linux__
	if (!SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "SDL_VIDEODRIVER", "kmsdrm", true))
	{
		Log(ERROR, "Failed to set SDL environment variable: %s", SDL_GetError());
		return -1;
	}
	#endif */

	uint8_t drivers_num = SDL_GetNumVideoDrivers();
	Log(INFO, "Detected video driver nummber : %u", drivers_num);
	for (uint8_t index = 0; index < drivers_num; index++)	Log(INFO, "Video Driver %d: %s", index, SDL_GetVideoDriver(index));

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		Log(ERROR, "Failed to initialize SDL3 core: %s", SDL_GetError());
		return -1;
	}

	Log(INFO, "Current Video Driver: %s.", SDL_GetCurrentVideoDriver());

	if (SDL_GetDisplayBounds(0, &display_bounds))
	{
		// (Width, Height):	(display_bounds.w, display_bounds.h)
		// Init Pos:		(display_bounds.x, display_bounds.y)

		Log(ERROR, "Failed to get display bounds: %s", SDL_GetError());
		SDL_Quit();
	}

	int count = 0;
	if ((display_mode = (SDL_DisplayMode *)SDL_GetCurrentDisplayMode(SDL_GetDisplays(&count)[0])) == NULL)	// '=', not "==".
	{
		Log(ERROR, "Failed to get display mode: %s", SDL_GetError());
		SDL_Quit();
	}

	Log(INFO, "Current display mode: %dx%d @ %.2lfHz.", display_mode->w, display_mode->h, display_mode->refresh_rate);

	if (!(window = SDL_CreateWindow("Tensor VM <Arch: 8086>", display_bounds.w / 2, display_bounds.h / 2, SDL_WINDOW_NOT_FOCUSABLE)))
	{
		Log(ERROR, "Failed to create window: %s", SDL_GetError());
		SDL_Quit();
		return -1;
	}
	SDL_SetWindowPosition(window, display_bounds.x + display_bounds.w / 2, display_bounds.y);

	/* SDL_PropertiesID sdl_props = SDL_CreateProperties();
	SDL_SetPointerProperty(sdl_props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, window);
	SDL_SetBooleanProperty(sdl_props, SDL_PROP_RENDERER_CREATE_ACCELERATED_BOOLEAN, SDL_TRUE);
	SDL_SetBooleanProperty(sdl_props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_BOOLEAN, SDL_TRUE); */
	if (!(renderer = SDL_CreateRenderer(window, NULL)))
	{
		Log(ERROR, "Failed to create renderer: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	Log(INFO, "Initialized monitor <Engine: SDL3> Pos: (%d, %d), Mode: %dx%d.", display_bounds.x + display_bounds.w / 2, display_bounds.y, display_bounds.w / 2, display_bounds.h / 2);
	clear_screen();

	return 0;
}

void *monitor_thread(void *)
{
	while (monitor_on)
	{
		while (SDL_PollEvent(&sdl_event))
		{
			SDL_Delay(50);
			switch (sdl_event.type)
			{
				case SDL_EVENT_QUIT:	monitor_on = false; break;
				case SDL_EVENT_KEY_DOWN:
					if (sdl_event.key.scancode == SDL_SCANCODE_ESCAPE)	monitor_on = false;
					break;
			}
		}
	}

	return NULL;
}

void monitor_destroy(void)
{
	Log(INFO, "Disconnecting monitor.");
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
