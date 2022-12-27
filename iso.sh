#!/bin/sh
set -e
. ./build.sh

mkdir -p build/isodir/boot/grub

cp build/sysroot/boot/alpine.kernel build/isodir/boot/alpine.kernel
cat > build/isodir/boot/grub/grub.cfg << EOF
menuentry "Alpine Kernel" {
	multiboot2 /boot/alpine.kernel
}
EOF
grub-mkrescue -o build/myos.iso build/isodir
