/*
 * "higher-level" neopixel stuff.
 * our model:
 *  1. you write to an array with as many entries as there are pixels as much as you 
 *     want
 *  2. you call <neopix_flush> when you want to externalize this.
 *
 * Note:
 *  - out of bound writes are ignored.
 *  - do not allow interrupts during the flush!
 */
#include "rpi.h"
#include "neopixel.h"
#include "WS2812B.h"
#include "graphics.h"

neo_t neopix_init(uint8_t pin, unsigned npixel) {
    neo_t h;
    unsigned nbytes = sizeof *h + sizeof h->pixels[0] * npixel;
    h = (void*)kmalloc(sizeof *h + sizeof h->pixels[0] * npixel);
    memset(h, 0, nbytes);

    h->npixel = npixel;
    h->pin = pin;
    gpio_set_output(pin);

    return h;
}

// set pixel <pos> in <h> to {r,g,b}
void neopix_write(neo_t h, uint32_t pos, color_t color) {
    if (pos < h->npixel) {
        h->pixels[pos].red = color.red;
        h->pixels[pos].green = color.green;
        h->pixels[pos].blue = color.blue;
    }
}

void neopix_flush(neo_t h) { 
    // needs the rest of the code.
    for (int i = 0; i < h->npixel; i++) {
        pix_sendpixel(h->pin, h->pixels[i].red, h->pixels[i].green, h->pixels[i].blue);
    }
    // pix_flush(h->pin);
    for (int i = 0; i < h->npixel; i++) {
        h->pixels[i].red = 0;
        h->pixels[i].green = 0;
        h->pixels[i].blue = 0;
        pix_flush(h->pin);
    }

}

void neopix_fast_clear(neo_t h, unsigned n) {
    for(int i = 0; i < n; i++)
        pix_sendpixel(h->pin, 0, 0, 0);
    neopix_flush(h);
}

void neopix_clear(neo_t h) {
    neopix_fast_clear(h, h->npixel);
}