// Tensor VM - monitor/display_core.h

#ifndef DISPLAY_CORE_H
#define DISPLAY_CORE_H

void clear_screen(void);

#ifdef __linux__
int overlay_init(void);
void overlay_present(void);
void overlay_cleanup(void);
#endif

#endif
