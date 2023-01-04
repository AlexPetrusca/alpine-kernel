#!/bin/sh
set -e
. ./config.sh

echo "Cleaning..."

for PROJECT in $PROJECTS; do
  (cd $PROJECT && $MAKE clean)
done

rm -rf build
rm -f qemu.log kernel.dump