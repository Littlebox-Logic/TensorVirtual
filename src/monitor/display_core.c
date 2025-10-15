// Tensor VM - monitor/display_core.c

#include "display_core.h"
#include "monitor.h"
#include "../log.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __linux__
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#endif

#ifdef __linux__
void *fb_buffer;
int drm_fd;
drmModePlane *drm_mode_plane = NULL;
drmModeRes *drm_mode_res;
uint32_t fb_id;
struct drm_mode_create_dumb creq; 
SDL_Rect sdl_rect;
#endif

uint8_t		line_index = 0;
uint16_t	line_offset = 0;
text_node	text_head = NULL;
text_node	text_tail = NULL;

void clear_screen(bool line_reset)
{
	const uint8_t border_thickness = 1;
	SDL_FRect border_rects[4];
	SDL_FRect bg_rect = {0, 0, display_mode->w, display_mode->h};

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);			// Background.
	SDL_RenderClear(renderer);
	SDL_RenderFillRect(renderer, &bg_rect); 

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);	// Border.
	border_rects[0] = (SDL_FRect){0, 0, display_mode->w / 2, border_thickness};
	border_rects[1] = (SDL_FRect){0, display_mode->h / 2 - border_thickness, display_mode->w, border_thickness};
	border_rects[2] = (SDL_FRect){0, 0, border_thickness, display_mode->h / 2};
	border_rects[3] = (SDL_FRect){display_mode->w / 2 - border_thickness, 0, border_thickness, display_mode->h / 2};

	for (uint8_t index = 0; index < 4; index++)	SDL_RenderFillRect(renderer, &border_rects[index]);
	#ifdef __linux__
	// if (!strcmp(SDL_GetCurrentVideoDriver(), "kmsdrm"))	overlay_present();
	// else
	#endif
	{
		// SDL_FlushRenderer(renderer);
		SDL_RenderPresent(renderer);
	}

	if (line_reset)
	{
		line_index = 0;
		text_free();
	}
	// Log(INFO, "Cleared workspace screen.");
}

void text_free(void)
{
	text_node temp_node;
	text_tail = NULL;
	while (text_head)
	{
		temp_node = text_head;
		text_head = text_head->next;
		SDL_DestroyTexture(temp_node->texture);
		free(temp_node->rect);
		free(temp_node);
	}
}

void text_uproll(void)
{
	text_node pioneer = text_head;
	text_node dead_pioneer;
	clear_screen(false);

	while (pioneer)
	{
		if (pioneer->line)
		{
			pioneer->line--;
			pioneer->rect->y -= 28.0f;
			SDL_RenderTexture(renderer, pioneer->texture, NULL, pioneer->rect);
			pioneer = pioneer->next;
		}
		else
		{
			dead_pioneer = pioneer;
			pioneer = pioneer->next;
			text_head = pioneer;
			SDL_DestroyTexture(dead_pioneer->texture);
			free(dead_pioneer->rect);
			free(dead_pioneer);
		}
	}

	// SDL_FlushRenderer(renderer);
	SDL_RenderPresent(renderer);
}

void head_tail_fill(const char *string, char *temp_string, uint64_t *phead, uint64_t *ptail, size_t index)
{
	if (*ptail - *phead)
	{
		memset(temp_string, '\0', *ptail - *phead + 1);
		strncpy(temp_string, &string[*phead], *ptail - *phead);
		text_output(temp_string, 255, 255, 255, false);
	}

	*ptail = *phead = index + 1;
}

int print_m(const char *string)
{
	size_t len = strlen(string);
	uint64_t head = 0, tail = 0;
	char *temp_string;
	if (!(temp_string = (char *)malloc(len + 1)))
	{
		Log(ERROR, "Failed to print to monitor.");
		return -1;
	}

	for (size_t index = 0; index < len; index++)
	{
		switch (string[index])	// \033[m will be developing.
		{
			case '\t':
				head_tail_fill(string, temp_string, &head, &tail, index);
				for (uint8_t times = 0; times < 8 - (line_offset + 1) % 8; times++)	text_output(" ", 0, 0, 0, false);	// ts = 4 (althrough seemly 8).
				break;
			case '\n':
				head_tail_fill(string, temp_string, &head, &tail, index);
				text_output(" ", 255, 255, 255, true);
				break;
			case '\b':
				head_tail_fill(string, temp_string, &head, &tail, index);
				line_offset--;
				break;
			default:
				tail++;
		}
	}

	if (string[len - 1] != '\t' && string[len - 1] != '\n')
	{
		memset(temp_string, '\0', tail - head + 1);
		strncpy(temp_string, &string[head], tail - head);
		text_output(temp_string, 255, 255, 255, false);
	}

	free(temp_string);
	return 0;
}

void text_output(const char *text, uint8_t red, uint8_t green, uint8_t blue, bool nextline)
{
	if (line_index > display_mode->h / 59)
	{
		text_uproll();
		line_index--;
	}

	text_node new_node = NULL;
	SDL_Color sdl_fgcolor = (SDL_Color){red, green, blue};
	// SDL_Color sdl_bgcolor = (SDL_Color){0, 0, 0};
	SDL_Surface *text_surface = TTF_RenderText_Blended(default_font, text, strlen(text), sdl_fgcolor);
	if ((new_node = (text_node)malloc(sizeof(_text_node))) == NULL)
	{
		Log(ERROR, "Failed to render new text. malloc() returned NULL.");
		return;
	}

	if ((new_node->rect = (SDL_FRect *)malloc(sizeof(SDL_FRect))) == NULL)
	{
		Log(ERROR, "Failed to register memory for text rect.");
		free(new_node);
		return;
	}

	// SDL_Rect temp_rect = (SDL_Rect){10 + line_offset * 12, 10 + line_index * 28, text_surface->w, text_surface->h};

	// SDL_FillSurfaceRect(text_surface, NULL, 0x8600FF);//SDL_MapRGB(text_surface->format, NULL, 0, 0, 0));
	new_node->texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	new_node->rect->x = 10.0f + (float)(line_offset * 12);
	new_node->rect->y = 10.0f + (float)(line_index * 28);
	new_node->rect->w = (float)text_surface->w;
	new_node->rect->h = (float)text_surface->h;
	new_node->line = line_index;

	new_node->next = NULL;
	SDL_DestroySurface(text_surface);

	if (text_head == NULL)	text_head = new_node;
	if (text_tail != NULL)	text_tail->next = new_node;
	text_tail = new_node;

	SDL_RenderTexture(renderer, text_tail->texture, NULL, text_tail->rect);
	if (nextline && line_index <= display_mode->h / 28)
	{
		line_index++;
		line_offset = 0;
	}
	else	line_offset += strlen(text);

	// SDL_FlushRenderer(renderer);
	SDL_RenderPresent(renderer);
}

#ifdef __linux__
int overlay_init(void)
{
	drm_fd = open("/dev/dri/card1", O_RDWR | O_CLOEXEC);
	sdl_rect = (SDL_Rect){0, 0, display_mode->w / 2, display_mode->h / 2};
	for (uint8_t index = 0; index < drmModeGetPlaneResources(drm_fd)->count_planes; index++)
	{
		drmModePlane *temp_plane = drmModeGetPlane(drm_fd, drmModeGetPlaneResources(drm_fd)->planes[index]);

		if (true) //temp_plane->type == DRM_PLANE_TYPE_OVERLAY)// && (temp_plane->possible_crtcs & (1 << crtc_index)))
		{
			drm_mode_plane = temp_plane;
			Log(INFO, "Selected DRM mode plane: %u (type: overlay)", index);
			break;
		}
		Log(ERROR, "No usable DRM mode plane (expected type: overlay).");
	}
	drm_mode_res = drmModeGetResources(drm_fd);

	creq.width = display_mode->w / 2;
	creq.height = display_mode->h / 2;
	creq.bpp = 32;
	ioctl(drm_fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);

	struct drm_mode_map_dumb mreq = {0};
	mreq.handle = creq.handle;
	ioctl(drm_fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);

	fb_buffer = mmap(0, creq.size, PROT_READ | PROT_WRITE, MAP_SHARED, drm_fd, mreq.offset);
	drmModeAddFB(drm_fd, display_mode->w / 2, display_mode->h / 2, 24, 32, display_mode->w * 2, creq.handle, &fb_id);

	return 0;
}

void overlay_present(void)
{
	Log(INFO, "Overlay present.");
	SDL_Surface *surface;
	if (!(surface = SDL_RenderReadPixels(renderer, &sdl_rect)))
	{
		Log(ERROR, "Failed to read SDL3 pixels: %s", SDL_GetError());
		return;
	}
	memcpy(fb_buffer, surface->pixels, creq.size);
	Log(DEBUG, "%llu", creq.size);
	//SDL_GetRenderLogicalPresentationRect(renderer, fb_buffer);
	drmModeSetPlane(drm_fd, drm_mode_plane->plane_id, drm_mode_res->crtcs[0], fb_id, 0, display_mode->w / 2, 0, display_mode->w / 2, display_mode->h / 2, display_mode->w / 2, 0, display_mode->w / 2, display_mode->h / 2);
	SDL_DestroySurface(surface);
}

void overlay_cleanup(void)
{
	drmModeSetPlane(drm_fd, drm_mode_plane->plane_id, drm_mode_res->crtcs[0], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	munmap(fb_buffer, creq.size);
	close(drm_fd);
}
#endif
