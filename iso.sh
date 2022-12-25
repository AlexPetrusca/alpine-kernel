#!/bin/sh
set -e
. ./build.sh

mkdir -p build/isodir/boot/grub

cp build/sysroot/boot/myos.kernel build/isodir/boot/myos.kernel
cat > build/isodir/boot/grub/grub.cfg << EOF
menuentry "Alpine Kernel" {
	multiboot2 /boot/myos.kernel
}
EOF
grub-mkrescue -o build/myos.iso build/isodir
