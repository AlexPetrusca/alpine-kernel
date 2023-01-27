#!/bin/sh
set -e
. ./build.sh

echo "Building ISO..."

mkdir -p build/isodir/boot/grub

cp build/sysroot/boot/alpine.kernel build/isodir/boot/alpine.kernel
cat > build/isodir/boot/grub/grub.cfg << EOF
set timeout=0
menuentry "Alpine Kernel" {
	multiboot2 /boot/alpine.kernel
}
EOF
grub-mkrescue -o build/alpine.iso build/isodir 2>/dev/null
