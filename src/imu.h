#ifndef __IMU_H__
#define __IMU_H__

enum {
    SHRT_MAX = 32767,

    SAD_ADDR = 0b1101011, //p29

    //******** Registers ********

    WHO_AM_I_REG = 0x0F,
    WHO_AM_I_VAL = 0x69,

    CTRL1_XL = 0x10,
    CTRL2_G = 0x11,
    CTRL3_C = 0x12,
    CTRL9_XL = 0x18,
    CTRL10_C = 0x19,

    //******** General ********
    // p46
    odr_13hz = 0b0000,
    odr_26hz = 0b0001,
    odr_52hz = 0b0010,
    odr_104hz = 0b0011,
    odr_208hz = 0b0100,
    odr_416hz = 0b0101,
    odr_833hz = 0b0111,
    odr_166khz = 0b1000,
    odr_333khz = 0b1001,
    odr_666khz = 0b1010,

    //******** Accelerometer ********
    accel_2g = 0b00,
    accel_4g = 0b10,
    accel_8g = 0b11,
    accel_16g = 0b01,

    OUTX_L_XL = 0x28,
    OUTX_H_XL = 0x29,
    OUTY_L_XL = 0x2A,
    OUTY_H_XL = 0x2B,
    OUTZ_L_XL = 0x2C,
    OUTZ_H_XL = 0x2D,

    //******** Gyroscope ********
    gyro_245dps  = 0b00,
    gyro_500dps  = 0b01,
    gyro_1000dps = 0b10,
    gyro_2000dps = 0b11,

    OUTX_L_G = 0x22,
    OUTX_H_G = 0x23,
    OUTY_L_G = 0x24,
    OUTY_H_G = 0x25,
    OUTZ_L_G = 0x26,
    OUTZ_H_G = 0x27,
};


// it's easier to bundle these together.
typedef struct { short x,y,z; } imu_xyz_t;

static inline imu_xyz_t 
xyz_mk(short x, short y, short z);
void xyz_print(const char *msg, imu_xyz_t xyz);

// i2c helpers.

// read register <reg> from i2c device <addr>
uint8_t imu_rd(uint8_t addr, uint8_t reg);

// write register <reg> with value <v> 
void imu_wr(uint8_t addr, uint8_t reg, uint8_t v);

// <base_reg> = lowest reg in sequence. --- hw will auto-increment 
int imu_rd_n(uint8_t addr, uint8_t base_reg, uint8_t *v, uint32_t n);


void minimu9_who(uint8_t addr);

// do a hard reset
void minimu9_reset(uint8_t addr);


//********** Accelerometer **********

typedef struct {
    uint8_t addr;
    unsigned hz;
    unsigned g;
} accel_t;

// set to accel 2g (p14), bandwidth 20hz (p15)
accel_t minimu9_accel_init(uint8_t addr, unsigned accel_odr, unsigned accel_g);

int accel_has_data(const accel_t *h);

imu_xyz_t accel_rd(const accel_t *h);

// takes in raw data and scales it.
imu_xyz_t accel_scale(accel_t *h, imu_xyz_t xyz);


//********** Gyroscope **********

typedef struct {
    uint8_t addr;
    unsigned hz;

    // scale: for accel is in g, for gyro is in dps.
    unsigned dps;
} gyro_t;

gyro_t minimu9_gyro_init(uint8_t addr, unsigned gyro_odr, unsigned gyro_dps);

// use int or fifo to tell when data.
int gyro_has_data(const gyro_t *h);

imu_xyz_t gyro_rd(const gyro_t *h);

// milli dps: i don't think is correct?
imu_xyz_t gyro_scale(gyro_t *h, imu_xyz_t xyz);


//********** Helpers **********

// returns the raw value from the sensor: combine
// the low and the hi and sign extend (cast to short)
static short mg_raw(uint8_t lo, uint8_t hi) {
    return (short)(hi<<8|lo);
}

// returns milligauss, integer
static int mg_scaled(int v, int mg_scale) {
    return (v * 1000 * mg_scale) / SHRT_MAX;
}

static int mdps_scale_deg2(int deg, int dps) {
    // hack to get around no div
    if(dps == 245) 
        return (deg * 245) / SHRT_MAX;
    else if(dps == 500) 
        return (deg * 500) / SHRT_MAX;
    else
        panic("bad dps=%d\n", dps);
}

#endif