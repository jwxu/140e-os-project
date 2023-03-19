#ifndef __WS2812B__
#define __WS2812B__

#include "rpi.h"
#include "cycle-count.h"

// A+ CPU speed: 700MHz (700 million cycles per second)
#define MHz 700UL
#define GHz 1000UL

// convert nanoseconds to pi cycles: you need to modify this if you change
// the pi's clock speed.
#define ns_to_cycles(x) (unsigned) ((x * 7UL) / 10UL )
#define cycles_to_ns(x) (unsigned) ((x * 10UL) / 7UL )

enum { 
    // to send a 1: set pin high for T1H ns, then low for T0H ns.
    T1H = ns_to_cycles(700),        // Width of a 1 bit in ns
    T0H = ns_to_cycles(100),        // Width of a 0 bit in ns
    // to send a 0: set pin high for T1L ns, then low for T0L ns.
    T1L = ns_to_cycles(250),        // Width of a 1 bit in ns
    T0L = ns_to_cycles(750),        // Width of a 0 bit in ns

    // to make the LED switch to the new values, old the pin low for FLUSH ns
    FLUSH = ns_to_cycles(50 * 1000)    // how long to hold low to flush
};

/****************************************************************************
 * Support code.
 */

enum {
    GPIO_BASE = 0x20200000,
    GPIO_SET0  = (GPIO_BASE + 0x1C),
    GPIO_CLR0  = (GPIO_BASE + 0x28),
};


static inline void gpio_set_on_raw(unsigned pin) {
    PUT32(GPIO_SET0, (1 << pin));
}

static inline void gpio_set_off_raw(unsigned pin) {
    PUT32(GPIO_CLR0, (1 << pin));
}

// use cycle_cnt_read() to delay <n_cyc> cycles measured from <start_cyc>
static inline void delay_ncycles(unsigned start_cyc, unsigned n_cyc) {
    while (cycle_cnt_read() - start_cyc < n_cyc)
        ;
}

/****************************************************************************
 * The WS2812B protocol
 */

// write 1 for <ncycles>: since reading the cycle counter itself takes cycles
// you may need to add a constant to correct for this.
static inline void write_1(unsigned pin, unsigned ncycles) {
    gpio_set_on_raw(pin);
    delay_ncycles(cycle_cnt_read(), ncycles);
}

// write 0 for <ncycles>: since reading the cycle counter takes cycles you
// may need to add a constant to correct for it.
static inline void write_0(unsigned pin, unsigned ncycles) {
    gpio_set_off_raw(pin);
    delay_ncycles(cycle_cnt_read(), ncycles);
}

// implement T1H from the datasheet (call write_1 with the right delay)
static inline void t1h(unsigned pin) {
    write_1(pin, T1H);
}

// implement T0H from the datasheet (call write_0 with the right delay)
static inline void t0h(unsigned pin) {
    write_1(pin, T0H);
}

// implement T1L from the datasheet.
static inline void t1l(unsigned pin) {
    write_0(pin, T1L);
}

// implement T0L from the datasheed.
static inline void t0l(unsigned pin) {
    write_0(pin, T0L);
}

// implement RESET from the datasheet.
static inline void treset(unsigned pin) {
    write_0(pin, FLUSH);
}

/***********************************************************************************
 * public functions.
 */

// flush out the pixels.
static inline void pix_flush(unsigned pin) { 
    treset(pin); 
}

// transmit a {0,1} bit to the ws2812b
void send_bit(unsigned pin, unsigned b);
static inline void pix_sendbit(unsigned pin, uint8_t b) {
    if (b) {
        t1h(pin);
        t1l(pin);
    } else {
        // output("LOW");
        // send_bit(pin, b);
        t0h(pin);
        t0l(pin);
    }
}

// use pix_sendbit to send byte <b>
static void pix_sendbyte(unsigned pin, uint8_t b) {
    for (unsigned char mask = 1 << 7; mask != 0 ; mask >>= 1 ) { 
        pix_sendbit(pin, b & mask);
        // send_bit(pin, b & mask);
    }
}

// use pix_sendbyte to send bytes [<r> red, <g> green, <b> blue out on pin <pin>.
static inline void pix_sendpixel(unsigned pin, uint8_t r, uint8_t g, uint8_t b) {
    pix_sendbyte(pin, g);
    pix_sendbyte(pin, r);
    pix_sendbyte(pin, b);
}
#endif