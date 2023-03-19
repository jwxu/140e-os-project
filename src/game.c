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
    BALL_TYPE = 1,
    WIDTH = 800,
    HEIGHT = 480
};

struct GameObject *game_ball;

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

void init_ball() {
    int x_start = WIDTH/2;
    int y_start = HEIGHT - 50;
    int radius = 5;
    draw_circle(x_start, y_start, radius, WHITE, 1);

    game_ball->x = x_start;
    game_ball->y = y_start;
    game_ball->radius = radius;
    game_ball->type = BALL_TYPE;
    game_ball->active = 1;
}

void ball_move(int x_shift, int y_shift) {
    // draw_rectangle(game_ball->x - game_ball->radius, game_ball->y - game_ball->radius, 
    //             game_ball->x + game_ball->radius, game_ball->y - game_ball->radius, ORANGE, 1);
    // // int old_x = game_ball->x;
    // // int old_y = game_ball->y;
    // int new_x = game_ball->x + x_shift;
    // int new_y = game_ball->y + y_shift;
    // if (new_x < 0 || new_x > WIDTH || new_y < 0 || new_y > HEIGHT) {
    //     output("UH OH");
    //     init_ball();
    // } else {
    //     // output("MOVE BALL");
    //     // draw_circle(new_x, new_y, game_ball->radius, WHITE, 1);
    //     // delay_us(10);
    //     // draw_circle(new_x - x_shift, new_y - y_shift, game_ball->radius, ORANGE, 1);
        
    //     game_ball->x = new_x;
    //     game_ball->y = new_y;
    //     // output("X1: %d\n", game_ball->x - game_ball->radius);
    //     // output("X2: %d\n", game_ball->x + game_ball->radius);
    //     // output("Y1: %d\n", game_ball->y - game_ball->radius);
    //     // output("Y2: %d\n", game_ball->y + game_ball->radius);
    // }
}

void init_game() {
    init_blocks();
    init_ball();
}

void restart() {
    draw_solid(BLACK);
    init_game();
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

    char *title = "Tilt-It";
    int ball_x_shift = 0;
    int ball_y_shift = 0;
    while (1) {
        draw_string(WIDTH/2 - (4 * FONT_WIDTH), 0, title, YELLOW);

        init_game();

        imu_xyz_t xyz_raw_a = accel_rd(&accel);
        imu_xyz_t xyz_raw_g = gyro_rd(&gyro);
        int accel_x = mg_scaled(xyz_raw_a.x, accel.g);
        int accel_y = mg_scaled(xyz_raw_a.y, accel.g);
        if (accel_x > 0.1) {
            output("Tilting left\n");
        } 
        if (accel_x < -0.1) {
            output("Tilting right\n");
        } 
        if (accel_y > 0.1) {
            output("Tilting up\n");
        } 
        if (accel_y < -0.1) {
            output("Tilting down\n");
        }

        // neopix_write(neopix, 0, RED);
        // neopix_flush(neopix);

        delay_ms(1000);
    }
}