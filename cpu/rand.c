#include <rpios/gpio.h>

#define RNG_CONTROL     ((volatile unsigned int*)(MMIO_BASE+0x00104000))
#define RNG_STATUS      ((volatile unsigned int*)(MMIO_BASE+0x00104004))
#define RNG_DATA        ((volatile unsigned int*)(MMIO_BASE+0x00104008))
#define RNG_INT_MASK    ((volatile unsigned int*)(MMIO_BASE+0x00104010))

void
rand_init()
{
    *RNG_STATUS = 0x40000;
    *RNG_INT_MASK |= 1; //mask interrupt
    *RNG_CONTROL |= 1; //enable
}

unsigned int
rand(unsigned int min,
     unsigned int max)
{
    //may need to wait for entropy
    while (!((*RNG_STATUS) >> 24))
        asm volatile("nop");
    return *RNG_DATA % (max-min) + min;
}
