#!/bin/bash
export PATH=$HOME/kernel_build/arm-eabi-4.6/bin/:$PATH
export ARCH=arm
export SUBARCH=arm
export CROSS_COMPILE=$HOME/kernel_build/arm-eabi-4.6/bin/arm-eabi-
export LOCALVERSION=+

export INSTALL_MOD_PATH=../modules

make rk3188_pmp5785_l1_defconfig

#make modules_install

make -j2 zImage

#./mkkrnlimg ./arch/arm/boot/zImage kernel.img

#zip -r out ./kernel.img
#mv ./out.zip ../kernel3.0.36+JB.zip

