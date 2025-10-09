// Tensor VM - monitor/display_core.c

#include "display_core.h"
#include "monitor.h"
#include "../log.h"

#include <SDL3/SDL.h>

void clear_screen(void)
{
	const uint8_t border_thickness = 4;
	SDL_FRect border_rects[4];

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);			// Background.
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);	// Border.
	border_rects[0] = (SDL_FRect){0, 0, display_bounds.w / 2, border_thickness};
	border_rects[1] = (SDL_FRect){0, display_bounds.h / 2 - border_thickness, display_bounds.w, border_thickness};
	border_rects[2] = (SDL_FRect){0, 0, border_thickness, display_bounds.h / 2};
	border_rects[3] = (SDL_FRect){display_bounds.w / 2 - border_thickness, 0, border_thickness, display_bounds.h / 2};

	for (uint8_t index = 0; index < 4; index++)	SDL_RenderFillRect(renderer, &border_rects[index]);
	SDL_RenderPresent(renderer);

	Log(INFO, "Cleared workspace screen.");
}
