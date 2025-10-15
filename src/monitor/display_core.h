// Tensor VM - monitor/display_core.h

#ifndef DISPLAY_CORE_H
#define DISPLAY_CORE_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

void clear_screen(bool);
void text_free(void);
void hello(void);
void text_uproll(void);
void head_tail_fill(const char *, char *, uint64_t *, uint64_t *, size_t);
int print_m(const char *);
void text_output(const char *, uint8_t, uint8_t, uint8_t, bool);

typedef struct _Text_node
{
	SDL_Texture			*texture;
	SDL_FRect			*rect;
	struct _Text_node	*next;
	uint8_t				line;
} _text_node, *text_node;

#ifdef __linux__
int overlay_init(void);
void overlay_present(void);
void overlay_cleanup(void);
#endif

#endif
