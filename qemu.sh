#!/bin/sh
set -e
. ./iso.sh

echo "Running QEMU..."
qemu-system-x86_64 \
		-machine q35 \
		-smp 2 \
		-m 256M  \
		-usb -device qemu-xhci,id=xhci -device usb-mouse \
    -cdrom build/alpine.iso \
    $@
