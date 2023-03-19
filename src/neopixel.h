#ifndef __NEOPIXEL_H__
#define __NEOPIXEL_H__

#include "graphics.h"

struct neo_handle {
    uint8_t pin;      // output pin
    uint32_t npixel;  // number of pixesl.

    // struct hack
    color_t pixels[];
};

typedef struct neo_handle *neo_t;

// dynamically allocate structure for addressable light strip.
neo_t neopix_init(uint8_t pin, unsigned npixels);

// buffered write {r,g,b} into <h> at position <pos>: last write wins.  
// does not flush out to light array until you call <neopixel_flush>.
//
// ignores if out of bounds (or assert?)
void neopix_write(neo_t h, uint32_t pos, color_t color);
void neopix_flush(neo_t h);

// immediately write black/off (0,0,0) to every pixel upto 
// pixel number <upto>
void neopix_fast_clear(neo_t h, unsigned upto);
// same: but write all pixels in <h>
void neopix_clear(neo_t h);

#endif