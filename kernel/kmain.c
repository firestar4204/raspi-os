#include <rpios/delay.h>
#include <rpios/lfb.h>
#include <rpios/mailbox.h>
#include <rpios/rand.h>
#include <rpios/uart.h>
#include <rpios/sd.h>

extern unsigned char _end;

void
kmain()
{
    uart_init();
    rand_init();
    lfb_init();
    lfb_print(80, 80, "Hello, World!\n");

    if (sd_init() == SD_OK) {
        if (sd_readblock(0, &_end, 1)) {
            uart_dump(&_end);
        }
    }

    /* mbox[0] = 8*4;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = MBOX_TAG_GETSERIAL;
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 0;
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My serial number is: ");
        uart_hex(mbox[6]);
        uart_hex(mbox[5]);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query serial.\n");
    }

    uart_puts("Here is a random number: ");
    uart_hex(rand(0, 4294967295));
    uart_puts("\n");

    uart_puts("Waiting 1000000 CPU cycles... ");
    wait_cycles(1000000);
    uart_puts("Ok.\n");

    uart_puts("Waiting 1000000 uS... ");
    wait_msec(1000000);
    uart_puts("Ok. \n"); */

    lfb_print(80, 80, "Hello, World!\n");
    // lfb_proprint(80, 120, "Hello, World!\n");

    while (1) {
        uart_send(uart_getc());
    }
}