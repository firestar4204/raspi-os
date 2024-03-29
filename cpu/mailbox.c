#include <rpios/gpio.h>

volatile unsigned int __attribute__((aligned(16))) mbox[36];

#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x0))
#define MBOX_POLL       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x10))
#define MBOX_SENDER     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x14))
#define MBOX_STATUS     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x18))
#define MBOX_CONFIG     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x1C))
#define MBOX_WRITE      ((volatile unsigned int*)(VIDEOCORE_MBOX+0x20))
#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

//makes a mailbox call, returns 0 on failure, non-zero success
int
mbox_call(unsigned char ch)
{
    unsigned int r = (((unsigned int)((unsigned long)&mbox)&~0xF) | (ch&0xF));
    //wait until we can write
    do {
        asm volatile("nop");
    } while (*MBOX_STATUS & MBOX_FULL);
    //write the address of the message to the mailbox with channel id
    *MBOX_WRITE = r;
    //wait for response
    while (1) {
        do {
            asm volatile("nop");
        } while (*MBOX_STATUS & MBOX_EMPTY);
        if (r == *MBOX_READ)
            return mbox[1] == MBOX_RESPONSE;
    }
    return 0; //failure
}