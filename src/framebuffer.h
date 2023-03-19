#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

typedef struct {
        unsigned width;
        unsigned height;
        unsigned virtual_width;
        unsigned virtual_height;
        unsigned pitch;
        unsigned depth;
        unsigned x_offset;
        unsigned y_offset;
        void *framebuf_ptr;
        unsigned n_bytes;
} __attribute__ ((aligned(16))) fb_config_t;

static fb_config_t framebuf;

static const uint32_t FRAMEBUFFER_CHANNEL = 1;

static const uint32_t FRAMEBUF_WIDTH = 800;
static const uint32_t FRAMEBUF_HEIGHT = 480;
static const uint32_t FRAMEBUF_DEPTH = 24;

void framebuf_init(void);

uint32_t framebuf_get_width(void);

uint32_t framebuf_get_height(void);

uint32_t framebuf_get_depth(void);

uint32_t framebuf_get_pitch(void);

void * framebuf_get_buf_ptr(void);

#endif
