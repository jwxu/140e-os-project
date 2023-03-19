#ifndef __MAILBOX_H__
#define __MAILBOX_H__

enum {
    PERIPHERAL_BASE = 0x20000000,
    MAILBOX_BASE_ADDR = (PERIPHERAL_BASE + 0xB880),
    MAILBOX_READ = (MAILBOX_BASE_ADDR + 0x00),
    MAILBOX_PEAK = (MAILBOX_BASE_ADDR + 0x10),
    MAILBOX_SENDER = (MAILBOX_BASE_ADDR + 0x14),
    MAILBOX_STATUS = (MAILBOX_BASE_ADDR + 0x18),
    MAILBOX_CONFIG = (MAILBOX_BASE_ADDR + 0x1C),
    MAILBOX_WRITE = (MAILBOX_BASE_ADDR + 0x20),
    MAILBOX_FULL  = 1 << 31,
    MAILBOX_EMPTY  = 1 << 30,
    GPU_NOCACHE = 0x40000000
};

typedef struct {
  unsigned read;
  unsigned padding[3];
  unsigned peek;
  unsigned sender;
  unsigned status;
  unsigned configuration;
  unsigned write;
} mailbox_t;

static volatile mailbox_t *mailbox = (volatile mailbox_t *)MAILBOX_BASE_ADDR;

void write_mailbox(uint32_t msg, uint32_t channel);

uint32_t read_mailbox(uint32_t channel);

#endif