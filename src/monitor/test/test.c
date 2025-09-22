#include <SDL2/SDL.h>
#include <stdio.h>
 
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
 
int monitor_init(void)
{
	SDL_Window* monitor = NULL;
	SDL_Surface* screenSurface = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		exit(-1);
	}

	monitor = SDL_CreateWindow("Logic Tensor VM (x86 emulator)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (NULL == monitor)
	{
		SDL_Quit();
		return -1;
	}

	screenSurface = SDL_GetWindowSurface(monitor);
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
	SDL_UpdateWindowSurface(monitor);

    SDL_Delay(10000);

    SDL_DestroyWindow(monitor);
	SDL_Quit();
    return 0;
}
