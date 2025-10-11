// Tensor VM - monitor/display_core.c

#include "display_core.h"
#include "monitor.h"
#include "../log.h"

#include <SDL3/SDL.h>
#include <stdint.h>

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

void clear_screen(void)
{
	const uint8_t border_thickness = 1;
	SDL_FRect border_rects[4];
	SDL_FRect bg_rect = {0, 0, display_mode->w, display_mode->h};

	//SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);			// Background.
	//SDL_RenderClear(renderer);
	SDL_RenderFillRect(renderer, &bg_rect); 

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);	// Border.
	border_rects[0] = (SDL_FRect){0, 0, display_mode->w / 2, border_thickness};
	border_rects[1] = (SDL_FRect){0, display_mode->h / 2 - border_thickness, display_mode->w, border_thickness};
	border_rects[2] = (SDL_FRect){0, 0, border_thickness, display_mode->h / 2};
	border_rects[3] = (SDL_FRect){display_mode->w / 2 - border_thickness, 0, border_thickness, display_mode->h / 2};

	for (uint8_t index = 0; index < 4; index++)	SDL_RenderFillRect(renderer, &border_rects[index]);
	#ifdef __linux__
	if (!strcmp(SDL_GetCurrentVideoDriver(), "kmsdrm"))	overlay_present();
	else
	#endif
	{
		SDL_RenderPresent(renderer);
		SDL_Delay(16);
		SDL_RenderPresent(renderer);
	}
	// Log(INFO, "Cleared workspace screen.");
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
