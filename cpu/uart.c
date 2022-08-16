#include <rpios/gpio.h>
#include <rpios/mailbox.h>

#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))

void
uart_init()
{
    register unsigned int r;

    //init UART
    *UART0_CR = 0; //turn off UART0

    //set up clock
    mbox[0] = 9*4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_SETCLKRATE;
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2; //UART clock
    mbox[6] = 4000000; //4MHz
    mbox[7] = 0; //clear turbo
    mbox[8] = MBOX_TAG_LAST;
    mbox_call(MBOX_CH_PROP);

    //map UART0 to GPIO pins
    r = *GPFSEL1;
    r &= ~((7<<12)|(7<<15)); //gpio14, gpio15
    r |= (4<<12)|(4<<15); //alt0
    *GPFSEL1 = r;
    *GPPUD = 0; //enable pins 14 and 15
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = 0; //flush gpio setup

    *UART0_ICR = 0x7FF; //clear interrupts
    *UART0_IBRD = 2;
    *UART0_FBRD = 0xB;
    *UART0_LCRH = 0x7<<4;
    *UART0_CR = 0x301;
}

void
uart_send(unsigned int c)
{
    //wait until we can send
    do {
        asm volatile("nop");
    } while (*UART0_FR & 0x20);
    *UART0_DR = c;
}

char
uart_getc()
{
    char r;
    do {
        asm volatile("nop");
    } while (*UART0_FR&0x10);
    r = (char)(*UART0_DR);
    return r == '\r' ? '\n' : r;
}

void
uart_puts(char *s)
{
    while (*s) {
        if (*s == '\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

void
uart_hex(unsigned int d)
{
    unsigned int n;
    int c;
    for (c = 28; c >= 0; c -= 4) {
        //get highest tetrad
        n = (d >> c) & 0xF;
        n += n > 9 ? 0x37 : 0x30;
        uart_send(n);
    }
}

void
uart_dump(void *ptr)
{
    unsigned long a,b,d;
    unsigned char c;
    for(a=(unsigned long)ptr;a<(unsigned long)ptr+512;a+=16) {
        uart_hex(a); uart_puts(": ");
        for(b=0;b<16;b++) {
            c=*((unsigned char*)(a+b));
            d=(unsigned int)c;d>>=4;d&=0xF;d+=d>9?0x37:0x30;uart_send(d);
            d=(unsigned int)c;d&=0xF;d+=d>9?0x37:0x30;uart_send(d);
            uart_send(' ');
            if(b%4==3)
                uart_send(' ');
        }
        for(b=0;b<16;b++) {
            c=*((unsigned char*)(a+b));
            uart_send(c<32||c>=127?'.':c);
        }
        uart_send('\r');
        uart_send('\n');
    }
}
