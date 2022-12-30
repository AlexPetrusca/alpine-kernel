#!/bin/sh
set -e
. ./iso.sh

sudo balena local flash build/alpine.iso $@