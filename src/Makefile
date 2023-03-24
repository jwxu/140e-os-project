BOOTLOADER = my-install

# PROGS += tests/test_graphics.c
# PROGS += tests/test_neopixel_blink.c
# PROGS += tests/test_neopixel_cursor.c
# PROGS += tests/test_neopixel_cursor_2.c
# PROGS += tests/test_accel.c
PROGS += game.c

# CFLAG += -I../include

# make a local copy of cstart so you can modify it.
COMMON_SRC = mailbox.c framebuffer.c graphics.c
COMMON_SRC += neopixel.c
COMMON_SRC += imu.c

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

STAFF_OBJS += i2c.o

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template
