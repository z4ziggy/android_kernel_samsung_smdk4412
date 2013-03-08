#!/bin/bash

if [ -e boot.img ]; then
	rm boot.img
fi

if [ -e compile.log ]; then
	rm compile.log
fi

if [ -e ramdisk.cpio ]; then
	rm ramdisk.cpio
fi

# Set Default Path
TOP_DIR=$PWD
KERNEL_PATH="/home/dominik/android/android_4.2/kernel/samsung/smdk4412"

# Set toolchain and root filesystem path
#TOOLCHAIN_PATH="/home/dominik/android/android_4.2/prebuilt/linux-x86/toolchain/linaro/bin"
TOOLCHAIN_PATH="/home/dominik/android/android_4.2/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin"
TOOLCHAIN="$TOOLCHAIN_PATH/arm-eabi-"
ROOTFS_PATH="ramdisk-aosp-lte"

export KBUILD_BUILD_VERSION="Devil-N7105-CM-0.1"
export KERNELDIR=$KERNEL_PATH

export USE_SEC_FIPS_MODE=true

# Cleaning old kernel and modules
find -name '*.ko' -exec rm -rf {} \;
rm -rf $KERNEL_PATH/arch/arm/boot/zImage

# Making our .config
make cyanogenmod_t0lte_defconfig

make modules -j`grep 'processor' /proc/cpuinfo | wc -l` ARCH=arm CROSS_COMPILE=$TOOLCHAIN || exit -1

# Copying kernel modules

#find -name '*.ko' -exec cp -av {} $ROOTFS_PATH/lib/modules/ \;
#        for i in $ROOTFS_PATH/lib/modules/*; do $TOOLCHAIN_PATH/arm-eabi-strip --strip-unneeded $i;done;\

make zImage -j`grep 'processor' /proc/cpuinfo | wc -l` ARCH=arm CROSS_COMPILE=$TOOLCHAIN || exit -1

# Copy Kernel Image
rm -f $KERNEL_PATH/releasetools/tar/$KBUILD_BUILD_VERSION.tar
rm -f $KERNEL_PATH/releasetools/zip/$KBUILD_BUILD_VERSION.zip
cp -f $KERNEL_PATH/arch/arm/boot/zImage .

# Create ramdisk.cpio archive
cd $ROOTFS_PATH
find . | cpio -o -H newc > ../ramdisk.cpio
cd ..

# Make boot.img
./mkbootimg --kernel zImage --ramdisk ramdisk.cpio --board smdk4x12 --base 0x10000000 --pagesize 2048 --ramdiskaddr 0x11000000 -o $KERNEL_PATH/boot.img

# Copy boot.img
cp boot.img $KERNEL_PATH/releasetools/zip
cp boot.img $KERNEL_PATH/releasetools/tar

# Creating flashable zip and tar
cd $KERNEL_PATH
cd releasetools/zip
zip -0 -r $KBUILD_BUILD_VERSION.zip *
mkdir -p $KERNEL_PATH/release
mv *.zip $KERNEL_PATH/release
cd ..
cd tar
tar cf $KBUILD_BUILD_VERSION.tar boot.img && ls -lh $KBUILD_BUILD_VERSION.tar
mv *.tar $KERNEL_PATH/release

# Cleanup
rm $KERNEL_PATH/releasetools/zip/boot.img
rm $KERNEL_PATH/releasetools/tar/boot.img
rm $KERNEL_PATH/zImage
