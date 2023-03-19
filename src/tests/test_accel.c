#include "rpi.h"
#include "i2c.h"
#include "imu.h"


static void test_dps(int expected_i, uint8_t h, uint8_t l, int dps) {
    int s_i = mdps_scale_deg(mg_raw(l, h), dps);
    expected_i *= 1000;
    int tol = 5;
    if(!within(expected_i, s_i, tol))
        panic("expected %d, got = %d, scale=%d\n", expected_i, s_i, dps);
    else
        output("expected %d, got = %d, (within +/-%d) scale=%d\n", expected_i, s_i, tol, dps);
}

static void test_mg(int expected, uint8_t l, uint8_t h, unsigned g) {
    int s_i = mg_scaled(mg_raw(h,l),g);
    printk("expect = %d, got %d\n", expected, s_i);
    assert(s_i == expected);
}


void notmain(void) {
    delay_ms(100);   // allow time for device to boot up.
    i2c_init();
    delay_ms(100);   // allow time to settle after init.

    
    minimu9_who(SAD_ADDR);

    //  hard reset: it won't be when your pi reboots.
    minimu9_reset(SAD_ADDR);

    // first test that your scaling works.
    // this is device independent.
    test_mg(0, 0x00, 0x00, 2);
    test_mg(350, 0x16, 0x69, 2);
    test_mg(1000, 0x40, 0x09, 2);
    test_mg(-350, 0xe9, 0x97, 2);
    test_mg(-1000, 0xbf, 0xf7, 2);

    // device independent testing.
    int dps = 245;
    test_dps(0, 0x00, 0x00, dps);
    test_dps(100, 0x2c, 0xa4, dps);
    test_dps(200, 0x59, 0x49, dps);
    test_dps(-100, 0xd3, 0x5c, dps);

    accel_t accel = minimu9_accel_init(SAD_ADDR, odr_166khz, accel_2g);
    assert(accel.g==2);
    assert(accel.hz==1660);
    gyro_t gyro = minimu9_gyro_init(SAD_ADDR, odr_166khz, gyro_245dps);
    assert(gyro.dps==245);
    assert(gyro.hz==1660);
    for(int i = 0; i < 100; i++) {
        imu_xyz_t xyz_raw_a = accel_rd(&accel);
        output("reading %d\n", i);
        xyz_print("\traw", xyz_raw_a);
        xyz_print("\tscaled (milligaus: 1000=1g)", accel_scale(&accel, xyz_raw_a));

        imu_xyz_t xyz_raw_g = gyro_rd(&gyro);
        output("reading gyro %d\n", i);
        xyz_print("\traw", xyz_raw_g);
        xyz_print("\tscaled (milli dps)", gyro_scale(&gyro, xyz_raw_g));
        delay_ms(1000);
    }
    output("SUCCESS!\n");
}

// void notmain(void) {
//     delay_ms(100);   // allow time for device to boot up.
//     i2c_init();
//     delay_ms(100);   // allow time to settle after init.

//     // from application note.
//     uint8_t dev_addr = 0b1101000;

//     enum { 
//         WHO_AM_I_REG      = 0x75, 
//         // this is default: but seems we can get 0x71 too
//         WHO_AM_I_VAL1 = 0x70,       
//         WHO_AM_I_VAL2 = 0x71 
//     };

//     uint8_t v = imu_rd(dev_addr, WHO_AM_I_REG);
//     if(v != WHO_AM_I_VAL1 && v != WHO_AM_I_VAL2)
//         panic("Initial probe failed: expected %b or %b, got %b\n", 
//             WHO_AM_I_VAL1, WHO_AM_I_VAL2, v);
//     else
//         printk("SUCCESS: mpu-6500 acknowledged our ping: WHO_AM_I=%b!!\n", v);

//     // hard reset: it won't be when your pi reboots.
//     minimu9_reset(dev_addr);

//     // first test that your scaling works.
//     // this is device independent.
//     test_mg(0, 0x00, 0x00, 2);
//     test_mg(350, 0x16, 0x69, 2);
//     test_mg(1000, 0x40, 0x09, 2);
//     test_mg(-350, 0xe9, 0x97, 2);
//     test_mg(-1000, 0xbf, 0xf7, 2);

//     // part 1: get the accel working.
//     accel_t h = minimu9_accel_init(dev_addr, accel_2g);
//     assert(h.g==2);
//     for(int i = 0; i < 100; i++) {
//         imu_xyz_t xyz_raw = accel_rd(&h);
//         output("reading %d\n", i);
//         xyz_print("\traw", xyz_raw);
//         xyz_print("\tscaled (milligaus: 1000=1g)", accel_scale(&h, xyz_raw));

//         delay_ms(1000);
//     }
// }

