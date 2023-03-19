#include "rpi.h"
#include "graphics.h"
#include "framebuffer.h"
#include "neopixel.h"
#include "i2c.h"
#include "imu.h"

struct GameObject
{
    unsigned x;
    unsigned y;
    unsigned width;
    unsigned height;
    unsigned radius;
    unsigned type;
    unsigned active;
};

enum {
    BLOCK_TYPE = 0,
    PEN_TYPE = 1,
    WIDTH = 800,
    HEIGHT = 480
};

struct GameObject *game_pen;

void init_blocks() {
    color_t colors[15] = {RED, GREEN, RED, ORANGE, YELLOW, GREEN, ORANGE, BLUE, YELLOW, RED, BLUE, ORANGE, YELLOW, GREEN, BLUE};
    int block_width = 38;
    int block_height = 10;
    int gap = 2;
    int x_start = 0;
    int y_start = 20;
    int x_end = x_start + block_width;
    int y_end = y_start + block_height;
    int i = 0;
    while (x_end < WIDTH) {
        if (i > 15) {
            i = 0;
        }
        draw_rectangle(x_start, y_start, x_end, y_end, colors[i], 1);

        x_start = x_end + gap;
        x_end = x_start + block_width;
        i++;
    }
    x_start = 0;
    x_end = x_start + block_width;
    y_start = y_end + gap;
    y_end = y_start + block_height;

}

void init_pen(color_t color) {
    int x_start = WIDTH/2;
    int y_start = HEIGHT/2;
    int pen_width = 5;
    int pen_height = 5;
    draw_rectangle(x_start, y_start, x_start + pen_width, y_start + pen_height, color, 1);
    // int radius = 5;
    // draw_circle(x_start, y_start, radius, WHITE, 1);

    game_pen->x = x_start;
    game_pen->y = y_start;
    game_pen->width = pen_width;
    game_pen->height = pen_height;
    // game_pen->radius = radius;
    game_pen->type = PEN_TYPE;
    game_pen->active = 1;
}

void pen_move(int x_shift, int y_shift) {
    int new_x = game_pen->x + x_shift;
    int new_y = game_pen->y + y_shift;
    if (new_x < 0 || new_x > WIDTH || new_y < 0 || new_y > HEIGHT) {
        panic();
    } else {
        draw_rectangle(new_x, new_y, new_x + game_pen->width, new_y + game_pen->height, WHITE, 1);
        // draw_circle(new_x, new_y, game_pen->radius, WHITE, 1);
        game_pen->x = new_x;
        game_pen->y = new_y;
    }
}

void init_game(color_t pen_color, int obstacles) {
    char *title = "Sketch-Etch";
    draw_string(WIDTH/2 - (4 * FONT_WIDTH), 0, title, YELLOW);

    if (obstacles == 1) {
        init_blocks();
    }
    init_pen(pen_color);
}

void restart() {
    draw_solid(BLACK);
    init_game(WHITE, 0);
}

void notmain(void) {
    // Initialize i2c
    output("initialize I2C");
    delay_ms(100);   // allow time for device to boot up.
    i2c_init();
    delay_ms(100);   // allow time to settle after init.

    // Initialize accelerometer/gyroscope
    output("Initialize IMU\n");
    minimu9_who(SAD_ADDR);
    minimu9_reset(SAD_ADDR);
    accel_t accel = minimu9_accel_init(SAD_ADDR, odr_166khz, accel_2g);
    gyro_t gyro = minimu9_gyro_init(SAD_ADDR, odr_166khz, gyro_245dps);

    // Initialize framebuffer
    output("Initialize framebuffer\n");
    framebuf_init();

    // Initialize neopixels
    output("Initialize neopixel\n");
    // int pix_pin = 21;
    // gpio_set_output(pix_pin);
    // uint32_t npixels = 4;
    // neo_t neopix = neopix_init(pix_pin, npixels);

    int pen_x_shift = 0;
    int pen_y_shift = 0;
    int shift_amount = 4;

    init_game(WHITE, 0);

    while (1) {
        imu_xyz_t xyz_raw_a = accel_rd(&accel);
        imu_xyz_t xyz_raw_g = gyro_rd(&gyro);
        int accel_x = mg_scaled(xyz_raw_a.x, accel.g);
        int accel_y = mg_scaled(xyz_raw_a.y, accel.g);
        if (accel_x > 0.1) {
            // output("Tilting left\n");
            pen_x_shift = -shift_amount;
        } 
        if (accel_x < -0.1) {
            // output("Tilting right\n");
            pen_x_shift = shift_amount;
        } 
        if (accel_y > 0.1) {
            // output("Tilting up\n");
            pen_y_shift = shift_amount;
        } 
        if (accel_y < -0.1) {
            // output("Tilting down\n");
            pen_y_shift = -shift_amount;
        }

        pen_move(pen_x_shift, pen_y_shift);

        // neopix_write(neopix, 0, RED);
        // neopix_flush(neopix);

        delay_ms(500);
    }
}