### ducati_FreeRTOS ###

currently used compiler is arm-linux-gnueabihf. If yours is different, you will 
need to update the newlib ./configure arguments and Makefile to match your 
toolchain.

## building newlib

This is based on newlib version 1.20.
source : ftp://sources.redhat.com/pub/newlib/newlib-1.20.00.tar.gz

you need to trigger the build from inside the newlib<version>/newlib/ directory,
newlib is autotool'd :

$ ./configure  --host=arm-linux-gnueabihf --target=arm-linux-gnueabihf \
					--disable-newlib-supplied-syscalls \
					--disable-interwork --disable-multilib \
					CC=arm-linux-gnueabihf-gcc \
					CFLAGS='-march=armv7-m -mthumb -fno-stack-protector'
$ make

## building ducati_FreeRTOS

you need to build ducati_FreeRTOS with LDLIBS environnement variable :

$ cd ducati/
$ make LDLIBS=<path to newlib<version>/newlib/ directory>
