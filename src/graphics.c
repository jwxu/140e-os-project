#include "rpi.h"
#include "graphics.h"
#include "framebuffer.h"

void write_pixel(uint32_t x, uint32_t y, color_t pix) {
    uint8_t *position = framebuf_get_buf_ptr() + (y * framebuf_get_pitch()) + (x * BYTES_PER_PIXEL);
    memcpy(position, &pix, BYTES_PER_PIXEL);
}

void draw_solid(color_t pix) {
    int height = framebuf_get_height();
    int width = framebuf_get_width();
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            write_pixel(x, y, pix);
        }
	}   
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void draw_line(int x1, int y1, int x2, int y2, color_t pix) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int D = (2 * dy) - dx; // midpoint eval
    int y = y1;

    for (int x = x1; x < x2; x++) {
        write_pixel(x, y, pix);
        if (D > 0) {
            y++;
            D = D - 2*dx;
        }
        D = D + 2*dy;
    }
}

void draw_rectangle(int x1, int y1, int x2, int y2, color_t pix, int fill) {
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            if ((x == x1 || x == x2) || (y == y1 || y == y2)) {
                write_pixel(x, y, pix);
            } else if (fill) {
                write_pixel(x, y, pix);
            }
        }
    } 
}

void draw_circle(int x0, int y0, int r, color_t pix, int fill) {
    int x = r;
    int y = 0;
    int err = 0;
 
    while (x >= y) {
	if (fill) {
	   draw_line(x0 - y, y0 + x, x0 + y, y0 + x, pix);
	   draw_line(x0 - x, y0 + y, x0 + x, y0 + y, pix);
	   draw_line(x0 - x, y0 - y, x0 + x, y0 - y, pix);
	   draw_line(x0 - y, y0 - x, x0 + y, y0 - x, pix);
	}
	write_pixel(x0 - y, y0 + x, pix);
	write_pixel(x0 + y, y0 + x, pix);
	write_pixel(x0 - x, y0 + y, pix);
    write_pixel(x0 + x, y0 + y, pix);
	write_pixel(x0 - x, y0 - y, pix);
	write_pixel(x0 + x, y0 - y, pix);
	write_pixel(x0 - y, y0 - x, pix);
	write_pixel(x0 + y, y0 - x, pix);

	if (err <= 0) {
	    y += 1;
	    err += 2*y + 1;
	}
 
	if (err > 0) {
	    x -= 1;
	    err -= 2*x + 1;
	}
    }
}

void draw_char(unsigned char ch, int x, int y, color_t pix) {
    unsigned char *glyph = (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;
    color_t col = BLACK;
    for (int i=0; i < FONT_HEIGHT; i++) {
        for (int j=0; j < FONT_WIDTH; j++) {
            unsigned char mask = 1 << j;
            if (*glyph & mask) {
                col = pix;
            } else {
                col = BLACK;
            }
            write_pixel(x+j, y+i, col);
        }
        glyph += FONT_BPL;
    }
}

void draw_string(int x, int y, char *s, color_t pix) {
    while (*s) {
       if (*s == '\r') {
          x = 0;
       } else if(*s == '\n') {
          x = 0; y += FONT_HEIGHT;
       } else {
	  draw_char(*s, x, y, pix);
          x += FONT_WIDTH;
       }
       s++;
    }
}