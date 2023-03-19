#include "rpi.h"
#include "bit-support.h"
#include "mailbox.h"

void write_mailbox(uint32_t msg, uint32_t channel) {
    while (mailbox->status & MAILBOX_FULL)
        ;
    
    uint32_t write_val = 0;
    write_val = bits_set(write_val, 0, 3, channel);
    write_val = bits_set(write_val, 4, 31, msg >> 4);
    write_val |= GPU_NOCACHE;

    asm volatile ("" : : : "memory");
    mailbox->write = write_val;
}

uint32_t read_mailbox(uint32_t channel) {
    while (1) {
        while (mailbox->status & MAILBOX_EMPTY);
        
        uint32_t msg = mailbox->read;
        uint32_t msg_channel = bits_get(msg, 0, 3);
        if (msg_channel == channel) {
            return (msg >> 4);
        }
    }
    return 1;
}