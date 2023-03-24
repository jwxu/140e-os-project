#include "rpi.h"
#include "graphics.h"
#include "framebuffer.h"

void notmain(void) {
    framebuf_init();

	while (1) {
		draw_solid(OLIVE);
	}

    trace("SUCCESS!");
}
