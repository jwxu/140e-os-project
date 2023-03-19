#include "rpi.h"
#include "i2c.h"
#include "imu.h"
#include <limits.h>

// it's easier to bundle these together.

static inline imu_xyz_t 
xyz_mk(short x, short y, short z) {
    return (imu_xyz_t){.x = x, .y = y, .z = z};
}
void xyz_print(const char *msg, imu_xyz_t xyz) {
    output("%s (x=%d,y=%d,z=%d)\n", msg, xyz.x,xyz.y,xyz.z);
}

// i2c helpers.

// read register <reg> from i2c device <addr>
uint8_t imu_rd(uint8_t addr, uint8_t reg) {
    i2c_write(addr, &reg, 1);
        
    uint8_t v;
    i2c_read(addr,  &v, 1);
    return v;
}

// write register <reg> with value <v> 
void imu_wr(uint8_t addr, uint8_t reg, uint8_t v) {
    uint8_t data[2];
    data[0] = reg;
    data[1] = v;
    i2c_write(addr, data, 2);
    // printk("writeReg: %x=%x\n", reg, v);
}


// <base_reg> = lowest reg in sequence. --- hw will auto-increment 
int imu_rd_n(uint8_t addr, uint8_t base_reg, uint8_t *v, uint32_t n) {
        i2c_write(addr, (void*) &base_reg, 1);
        return i2c_read(addr, v, n);
}


//********** General **********

void minimu9_who(uint8_t addr) {
    uint8_t v = imu_rd(SAD_ADDR, WHO_AM_I_REG);
    if(v == WHO_AM_I_VAL)
        printk("SUCCESS: miniMU-9 v5 acknowledged our ping: WHO_AM_I=%b\n", v);
}

// do a hard reset
void minimu9_reset(uint8_t addr) {
    // reset: p49
    imu_wr(addr, 0b1, 1);

    // they don't give a delay; but it's typical you need one.
    delay_ms(100);
}


//********** Accelerometer **********

// set to accel 2g (p46), bandwidth 20hz (p15)
accel_t minimu9_accel_init(uint8_t addr, unsigned accel_odr, unsigned accel_g) {
    unsigned hz = 0;
    unsigned g = 0;
    switch (accel_odr) {
        case odr_13hz: hz = 13; break;
        case odr_26hz: hz = 26; break;
        case odr_52hz: hz = 52; break;
        case odr_104hz: hz = 104; break;
        case odr_208hz: hz = 208; break;
        case odr_416hz: hz = 416; break;
        case odr_833hz: hz = 833; break;
        case odr_166khz: hz = 1660; break;
        case odr_333khz: hz = 3330; break;
        case odr_666khz: hz = 6660; break;
        default: panic("invalid hz=%b\n", hz);
    }
    switch(accel_g) {
        case accel_2g: g = 2; break;
        case accel_4g: g = 4; break;
        case accel_8g: g = 8; break;
        case accel_16g: g = 16; break;
        default: panic("invalid g=%b\n", g);
    }

    // Set hz and g
    unsigned val = (accel_odr << 4) | (accel_g << 2);
    imu_wr(addr, CTRL1_XL, val);

    // Enable
    imu_wr(addr, CTRL9_XL, 0b111000);

    return (accel_t) { .addr = addr, .g = g, .hz = hz };
}

int accel_has_data(const accel_t *h) {
    return 1;
}

imu_xyz_t accel_rd(const accel_t *h) {
    // not sure if we have to drain the queue if there are more readings?

    unsigned mg_scale = h->g;
    uint8_t addr = h->addr;

    // right now this doesn't do anything.
    while(!accel_has_data(h))
        ;

    // read in the x,y,z from the accel using imu_rd_n
    short x = 0, y = 0, z = 0;

    x = (imu_rd(h->addr, OUTX_H_XL) << 8) | imu_rd(h->addr, OUTX_L_XL);
    y = (imu_rd(h->addr, OUTY_H_XL) << 8) | imu_rd(h->addr, OUTY_L_XL);
    z = (imu_rd(h->addr, OUTZ_H_XL) << 8) | imu_rd(h->addr, OUTZ_L_XL);

    return xyz_mk(x,y,z);
}

// takes in raw data and scales it.
imu_xyz_t accel_scale(accel_t *h, imu_xyz_t xyz) {
    int g = h->g;
    int x = mg_scaled(xyz.x, g);
    int y = mg_scaled(xyz.y, g);
    int z = mg_scaled(xyz.z, g);
    return xyz_mk(x,y,z);
}

//********** Gyroscope **********

gyro_t minimu9_gyro_init(uint8_t addr, unsigned gyro_odr, unsigned gyro_dps) { 
    unsigned dps = 0;
    unsigned hz = 0;
    switch (gyro_odr) {
        case odr_13hz: hz = 13; break;
        case odr_26hz: hz = 26; break;
        case odr_52hz: hz = 52; break;
        case odr_104hz: hz = 104; break;
        case odr_208hz: hz = 208; break;
        case odr_416hz: hz = 416; break;
        case odr_833hz: hz = 833; break;
        case odr_166khz: hz = 1660; break;
        default: panic("invalid hz=%b\n", hz);
    }
    switch(gyro_dps) {
        case gyro_245dps: dps = 245; break;
        case gyro_500dps: dps = 500; break;
        case gyro_1000dps: dps = 1000; break;
        case gyro_2000dps: dps = 2000; break;
        default: panic("invalid dps: %b\n", dps);
    }

    // Set hz and dps
    unsigned val = (gyro_odr << 4) | (gyro_dps << 2);
    imu_wr(addr, CTRL2_G, val);

    // Enable
    imu_wr(addr, CTRL10_C, 0b111000);

    return (gyro_t) { .addr = addr, .dps = dps, .hz = hz };
}

// use int or fifo to tell when data.
int gyro_has_data(const gyro_t *h) {
    return 1;
}


imu_xyz_t gyro_rd(const gyro_t *h) {
    // not sure if we have to drain the queue if there are more readings?

    unsigned dps_scale = h->dps;
    uint8_t addr = h->addr;

    while(!gyro_has_data(h))
        ;

    short x = 0, y = 0, z = 0;

    x = (imu_rd(h->addr, OUTX_H_G) << 8) | imu_rd(h->addr, OUTX_L_G);
    y = (imu_rd(h->addr, OUTY_H_G) << 8) | imu_rd(h->addr, OUTY_L_G);
    z = (imu_rd(h->addr, OUTZ_H_G) << 8) | imu_rd(h->addr, OUTZ_L_G);

    return xyz_mk(x,y,z);
}

// milli dps: i don't think is correct?
imu_xyz_t gyro_scale(gyro_t *h, imu_xyz_t xyz) {
    int dps = h->dps;
    int x = mdps_scale_deg2(xyz.x, dps);
    int y = mdps_scale_deg2(xyz.y, dps);
    int z = mdps_scale_deg2(xyz.z, dps);
    return xyz_mk(x,y,z);
}

