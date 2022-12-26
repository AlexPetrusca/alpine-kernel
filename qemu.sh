#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) \
		-machine q35 \
		-smp 2 \
		-m 256M  \
		-usb -device usb-ehci,id=ehci -device usb-mouse,bus=usb-bus.0 \
    -cdrom build/myos.iso
