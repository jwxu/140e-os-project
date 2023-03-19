#include "rpi.h"
#include "mailbox.h"
#include "framebuffer.h"

void framebuf_init(void) {
    framebuf.width = framebuf.virtual_width = FRAMEBUF_WIDTH;
    framebuf.height = framebuf.virtual_height = FRAMEBUF_HEIGHT;
    framebuf.pitch = 0;
    framebuf.depth = FRAMEBUF_DEPTH;
    framebuf.x_offset = framebuf.y_offset = 0;
    framebuf.framebuf_ptr = 0;
    framebuf.n_bytes = 0;

    write_mailbox((uint32_t)&framebuf, FRAMEBUFFER_CHANNEL);
}

uint32_t framebuf_get_width(void) {
    return framebuf.width;
}

uint32_t framebuf_get_height(void) {
    return framebuf.height;
}

uint32_t framebuf_get_depth(void) {
    return framebuf.depth;
}

uint32_t framebuf_get_pitch(void) {
    return framebuf.pitch;
}

void * framebuf_get_buf_ptr(void) {
    return (void *)framebuf.framebuf_ptr;
}
