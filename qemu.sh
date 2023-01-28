#!/bin/sh
set -e
. ./iso.sh

if [ "$1" = '-d' ]; then
  x86_64-elf-objdump -M intel -D kernel/build/alpine.kernel > kernel.dump
  debug=" -D qemu.log -d int"
  echo "Debugging QEMU..."
else
  debug=""
  echo "Running QEMU..."
fi

qemu-system-x86_64                                    \
		-machine q35                                      \
		-smp 1                                            \
		-m 256M                                           \
		-usb -device qemu-xhci,id=xhci -device usb-mouse  \
    -cdrom build/alpine.iso                           \
    $debug