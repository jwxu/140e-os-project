#include "rpi.h"
#include "graphics.h"
#include "framebuffer.h"
#include "neopixel.h"
#include "i2c.h"
#include "imu.h"
#include "math.h"

struct GameObject
{
    unsigned x;
    unsigned y;
    unsigned width;
    unsigned height;
    unsigned radius;
    color_t color;
};

enum {
    BLOCK_TYPE = 0,
    PEN_TYPE = 1,
    WIDTH = 800,
    HEIGHT = 480,
    BORDER_WIDTH = 50,
    BORDER_WIDTH_PAD = 57
};


struct GameObject *game_pen;

// void init_blocks() {
//     color_t colors[15] = {RED, GREEN, RED, ORANGE, YELLOW, GREEN, ORANGE, BLUE, YELLOW, RED, BLUE, ORANGE, YELLOW, GREEN, BLUE};
//     int block_width = 38;
//     int block_height = 10;
//     int gap = 2;
//     int x_start = 0;
//     int y_start = 20;
//     int x_end = x_start + block_width;
//     int y_end = y_start + block_height;
//     int i = 0;
//     while (x_end < WIDTH) {
//         if (i > 15) {
//             i = 0;
//         }
//         draw_rectangle(x_start, y_start, x_end, y_end, colors[i], 1);

//         x_start = x_end + gap;
//         x_end = x_start + block_width;
//         i++;
//     }
//     x_start = 0;
//     x_end = x_start + block_width;
//     y_start = y_end + gap;
//     y_end = y_start + block_height;

// }

void init_pen(color_t color) {
    int x_start = WIDTH/2;
    int y_start = HEIGHT/2;
    int pen_width = 6;
    int pen_height = 6;
    draw_rectangle(x_start, y_start, x_start + pen_width, y_start + pen_height, color, 1);
    // int radius = 5;
    // draw_circle(x_start, y_start, radius, WHITE, 1);

    game_pen->x = x_start;
    game_pen->y = y_start;
    game_pen->width = pen_width;
    game_pen->height = pen_height;
    // game_pen->radius = radius;
    game_pen->color = WHITE;
}

void pen_move(int x_shift, int y_shift) {
    int new_x = game_pen->x + x_shift;
    int new_y = game_pen->y + y_shift;
    if (new_x > BORDER_WIDTH_PAD && new_x < WIDTH - BORDER_WIDTH_PAD) {
        game_pen->x = new_x;
    }
    if (new_y > BORDER_WIDTH_PAD  && new_y < HEIGHT - BORDER_WIDTH_PAD) {
        game_pen->y = new_y;
    }
    // draw_circle(new_x, new_y, game_pen->radius, WHITE, 1);
    draw_rectangle(game_pen->x, game_pen->y, game_pen->x + game_pen->width, game_pen->y + game_pen->height, game_pen->color, 1);
}

void init_game(color_t pen_color, int obstacles) {
    draw_solid(RED);

    char *title = "Etch-A-Sketch";
    char *subtitle1 = "MAGIC";
    char *subtitle2 = "SCREEN";
    int zoom = 3;
    draw_string(WIDTH/2 - (6 * FONT_WIDTH * zoom) - 1, 12, title, YELLOW, RED, zoom);
    draw_string(WIDTH/4 - (2 * FONT_WIDTH), 15, subtitle1, YELLOW, RED, 1);
    draw_string((WIDTH/4 * 3) - (3 * FONT_WIDTH), 15, subtitle2, YELLOW, RED, 1);

    // Draw border
    draw_rectangle(BORDER_WIDTH, BORDER_WIDTH, WIDTH - BORDER_WIDTH, HEIGHT - BORDER_WIDTH, BLACK, 1);
    draw_circle(30, HEIGHT - 30, 25, WHITE, 1);
    draw_circle(WIDTH - 30, HEIGHT - 30, 25, WHITE, 1);

    // if (obstacles == 1) {
    //     init_blocks();
    // }
    init_pen(pen_color);
}

void clear_drawing(color_t new_color) {
    // output("Clear drawing\n");
    draw_rectangle(BORDER_WIDTH, BORDER_WIDTH, WIDTH - BORDER_WIDTH, HEIGHT - BORDER_WIDTH, BLACK, 1);
    game_pen->x = WIDTH/2;
    game_pen->y = HEIGHT/2;

    // New color
    game_pen->color = new_color;
}

void restart() {
    draw_solid(BLACK);
    init_game(WHITE, 0);
}

void notmain(void) {
    // Initialize i2c
    output("initialize I2C\n");
    delay_ms(100);   // allow time for device to boot up.
    i2c_init();
    delay_ms(100);   // allow time to settle after init.

    // Initialize accelerometer/gyroscope
    output("Initialize IMU\n");
    minimu9_who(SAD_ADDR);
    minimu9_reset(SAD_ADDR);
    accel_t accel = minimu9_accel_init(SAD_ADDR, odr_166khz, accel_2g);
    gyro_t gyro = minimu9_gyro_init(SAD_ADDR, odr_166khz, gyro_245dps);
    // minimu9_shake_init(SAD_ADDR, 1, 1);
    minimu9_ped_init(SAD_ADDR);

    // Initialize framebuffer
    output("Initialize framebuffer\n");
    framebuf_init();

    // Initialize neopixels
    output("Initialize neopixel\n");
    int pix_pin = 21;
    gpio_set_output(pix_pin);
    uint32_t npixels = 16;
    neo_t neopix = neopix_init(pix_pin, npixels);
    int pix_index = 0;

    color_t colors[15] = {RED, GREEN, BLUE, ORANGE, YELLOW, 
                        WHITE, GREEN, ORANGE, BLUE, YELLOW, 
                        RED, BLUE, ORANGE, YELLOW, GREEN};

    int pen_x_shift = 0;
    int pen_y_shift = 0;
    int shift_amount = 3;

    output("Initialize game\n");
    init_game(WHITE, 0);

    output("Drawing...\n");
    int num_shakes = 0;
    int time_elapsed = 0;
    int color_num = 0;
    while (1) {
        num_shakes += ped_read(SAD_ADDR);
        if (num_shakes > 20) {
            clear_drawing(colors[mod(color_num, 15)]);
            num_shakes = 0;
            color_num++;
            ped_reset(SAD_ADDR);
        } else if (time_elapsed > 5000 && num_shakes < 20) {
            num_shakes = 0;
            time_elapsed = 0;
            ped_reset(SAD_ADDR);
        }

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


        // place_neopix(neopix, pix_index, RED);
        neopix_write(neopix, pix_index, game_pen->color);
        neopix_flush(neopix);
        pix_index++;
        if (pix_index > npixels) {
            pix_index = 0;
        }

        time_elapsed += 500;
        delay_ms(500);
    }
}