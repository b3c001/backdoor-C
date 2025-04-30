#!/bin/bash

set -e
make clean && make
sudo insmod bico_lkm.ko
dmesg | tail
