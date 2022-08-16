SRCS = $(wildcard kernel/*.c drivers/*.c cpu/*.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

all: clean kernel8.img

boot/start.o: boot/start.S
	aarch64-elf-gcc $(CFLAGS) -c boot/start.S -o boot/start.o

%.o: %.c
	aarch64-elf-gcc $(CFLAGS) -Iinclude -c $< -o $@

font_psf.o: font.psf
	aarch64-elf-ld -r -b binary -o $@ $^

font_sfn.o: font.sfn
	aarch64-elf-ld -r -b binary -o $@ $^

kernel8.img: boot/start.o font_psf.o font_sfn.o $(OBJS)
	aarch64-elf-ld --nostdlib boot/start.o font_psf.o font_sfn.o $(OBJS) -T link.ld -o kernel8.elf
	aarch64-elf-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.* boot/start.o font_*.o $(OBJS) >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial stdio
