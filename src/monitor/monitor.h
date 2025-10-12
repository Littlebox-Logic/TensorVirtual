// Tensor VM - monitor/monitor.h

#ifndef MONITOR_H
#define MONITOR_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

int monitor_init(void);
void *monitor_thread(void *);
void monitor_destroy(void);

extern bool monitor_on;

extern SDL_Window *window; 
extern SDL_Renderer *renderer;
extern SDL_Rect display_bounds;
extern SDL_Event sdl_event;
extern SDL_DisplayMode *display_mode;

extern TTF_Font *default_font;

#endif
