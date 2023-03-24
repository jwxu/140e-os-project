/*
 * simple test to use your buffered neopixel interface to push a cursor around
 * a light array.
 */
#include "rpi.h"
#include "WS2812B.h"
#include "neopixel.h"
#include "graphics.h"

// the pin used to control the light strip.
enum { pix_pin = 21 };

// crude routine to write a pixel at a given location.
void place_cursor(neo_t h, int i) {
    neopix_write(h, i-4, BLUE);
    neopix_write(h, i-3, GREEN);
    neopix_write(h, i-2, YELLOW);
    neopix_write(h, i-1, ORANGE);
    neopix_write(h, i, RED);
    neopix_flush(h);
}

void notmain(void) {
    // enable_cache(); 
    gpio_set_output(pix_pin);

    // make sure when you implement the neopixel 
    // interface works and pushes a pixel around your light
    // array.
    unsigned npixels = 8;  // you'll have to figure this out.
    neo_t h = neopix_init(pix_pin, npixels);

    // does 10 increasingly faster loops.
    for(int j = 0; j < 10; j++) {
        output("loop %d\n", j);
        for(int i = 0; i < npixels; i++) {
            place_cursor(h,i);
            // delay_ms(10-j);
            delay_ms(500);
        }
    }
    // int on_index = 0;
    // for (int i = 0; i < npixels; i++) {
    //     color_t c = (i == on_index ? RED : BLUE);
    //     // color_t c = RED;
    //     // output("SEND PIX: %x\n", c);
    //     pix_sendpixel(h->pin, c.red, c.green, c.blue);
    // }
    // for (int i = 0; i < npixels; i++) {
    //     // color_t c = (i == on_index ? RED : BLUE);
    //     color_t c = RED;
    //     // output("SEND PIX: %x\n", c);
    //     pix_sendpixel(h->pin, c.red, c.green, c.blue);
    // }
    // for (int i = 0; i < npixels; i++) {
    //     // color_t c = (i == on_index ? RED : BLUE);
    //     color_t c = BLUE;
    //     // output("SEND PIX: %x\n", c);
    //     pix_sendpixel(h->pin, c.red, c.green, c.blue);
    // }
    // for (int i = 0; i < npixels; i++) {
    //     // color_t c = (i == on_index ? RED : BLUE);
    //     color_t c = GREEN;
    //     // output("SEND PIX: %x\n", c);
    //     pix_sendpixel(h->pin, c.red, c.green, c.blue);
    // }
    // pix_flush(h->pin);
    // // nleds must be power of 2
    // on_index = (on_index + 1) % npixels;
    output("done!\n");
}
