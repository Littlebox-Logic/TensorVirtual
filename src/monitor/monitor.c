// Tensor VM - monitor/monitor.c

#include "monitor.h"
#include "display_core.h"
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
	if (!SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "SDL_RENDER_DRIVER", "software", true))
	{
		Log(ERROR, "Failed to set SDL environment variable: %s", SDL_GetError());
		return -1;
	}

	uint8_t drivers_num = SDL_GetNumVideoDrivers();
	Log(INFO, "Detected video driver number : %u", drivers_num);
	for (uint8_t index = 0; index < drivers_num; index++)	Log(INFO, "Video Driver %d: %s", index, SDL_GetVideoDriver(index));

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		Log(ERROR, "Failed to initialize SDL3 core: %s", SDL_GetError());
		return -1;
	}

	Log(INFO, "Current Video Driver: %s.", SDL_GetCurrentVideoDriver());

	int count = 0;
	if ((display_mode = (SDL_DisplayMode *)SDL_GetCurrentDisplayMode(SDL_GetDisplays(&count)[0])) == NULL)	// '=', not "==".
	{
		Log(ERROR, "Failed to get display mode: %s", SDL_GetError());
		SDL_Quit();
	}

	Log(INFO, "Current display mode: %dx%d @ %.2lfHz.", display_mode->w, display_mode->h, display_mode->refresh_rate);
	if (!(window = SDL_CreateWindow("Tensor VM <Arch: 8086>", display_mode->w / 2, display_mode->h / 2, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_NOT_FOCUSABLE | SDL_WINDOW_ALWAYS_ON_TOP)))
	{
		Log(ERROR, "Failed to create window: %s", SDL_GetError());
		SDL_Quit();
		return -1;
	}
	SDL_SetWindowPosition(window, display_mode->w / 2, 0);

	if (!(renderer = SDL_CreateRenderer(window, NULL)))
	{
		Log(ERROR, "Failed to create renderer: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	#ifdef __linux__
	SDL_Rect viewport;
	if (!strcmp(SDL_GetCurrentVideoDriver(), "kmsdrm"))
	{
		viewport = (SDL_Rect){(float)(display_mode->w / 2), 0.0, (float)(display_mode->w / 2), (float)(display_mode->h / 2)};
		SDL_SetRenderViewport(renderer, &viewport);
		Log(INFO, "Set viewport for KMSDRM mode.");
	}
	#endif

	Log(INFO, "Initialized monitor <Engine: SDL3> Pos: (%d, %d), Mode: %dx%d.", display_mode->w / 2, 0, display_mode->w / 2, display_mode->h / 2);
	clear_screen();

	return 0;
}

void *monitor_thread(void *)
{
	while (monitor_on)
	{
		while (SDL_PollEvent(&sdl_event))
		{
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
