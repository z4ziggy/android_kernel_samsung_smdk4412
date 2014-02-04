#!/bin/bash

TARGET=$1
if [ "$TARGET" != "" ]; then
	echo "starting your build for $TARGET"
else
	echo ""
	echo "you need to define your device target (eg, i9300/i9100/note)"
	echo "example: $0 i9300"
	exit 1
fi

if [ "$TARGET" == "i9300" ] ; then
	CUSTOM_PATH=i9300
	MODE=DUAL
elif [ "$TARGET" == "i9100" ] ; then
	CUSTOM_PATH=i9100
	MODE=CM
else
	CUSTOM_PATH=note
	MODE=DUAL
fi

# setting vars
displayversion=zKernel-0.1
version=$displayversion-$TARGET-$MODE-$(date +%Y%m%d)
MAKE="make -j`grep 'processor' /proc/cpuinfo | wc -l`"
KERNEL_PATH=$PWD
TOOLCHAIN="arm-eabi-"
ROOTFS_PATH="$KERNEL_PATH/ramdisks/$TARGET-combo"
MODULESDIR="$KERNEL_PATH/ramdisks/modules"
MODULES="$KERNEL_PATH/ramdisks/modules/lib/modules"

defconfig=zkernel_"$TARGET"_defconfig

# exporting vars for make
export LOCALVERSION="-$displayversion"
export KERNELDIR=$KERNEL_PATH
export CROSS_COMPILE=$TOOLCHAIN
export ARCH=arm

export USE_SEC_FIPS_MODE=true

# Set ramdisk files permissions
cd $ROOTFS_PATH
ls $ROOTFS_PATH/roms/ | while read ramdisk; do
	cd $ROOTFS_PATH/roms/$ramdisk
	echo fixing permisions on $(pwd)
	chmod 644 *.rc
	chmod 750 init*
	chmod 640 fstab*
	chmod 644 default.prop
	chmod 750 $ROOTFS_PATH/sbin/init*
	chmod a+x $ROOTFS_PATH/sbin/*.sh
done
cd $KERNEL_PATH

rm boot.img compile.log ramdisk.cpio ramdisk.cpio.gz >/dev/null 2>&1
if [ "$2" == "clean" ]; then
	echo "Cleaning latest build"
	$MAKE distclean
fi
# Clean, generate .config, and build
$MAKE clean
$MAKE $defconfig
$MAKE || exit -1

# Copying and stripping kernel modules
if [ "$TARGET" == "i9100" ] ; then
	MODULES=releasetools/$CUSTOM_PATH/zip/system/lib/modules
fi

mkdir -p $MODULES
find -name '*.ko' -exec cp -av {} $MODULES \; 2>/dev/null
${TOOLCHAIN}strip --strip-unneeded $MODULES/*

# Copy Kernel Image
rm -f $KERNEL_PATH/releasetools/$CUSTOM_PATH/tar/$version.tar
rm -f $KERNEL_PATH/releasetools/$CUSTOM_PATH/zip/$version.zip
cp -f $KERNEL_PATH/arch/arm/boot/zImage .

if [ "$TARGET" != "i9100" ] ; then
	# Create ramdisk.cpio archive
	cd $MODULESDIR
	find . | fakeroot cpio -o -H newc > $KERNEL_PATH/ramdisk.cpio 2>/dev/null
	gzip -9 $KERNEL_PATH/ramdisk.cpio
	#ls --color=auto -lh $KERNEL_PATH/ramdisk.cpio.gz
	cd $KERNEL_PATH

	# Make boot.img
	./mkbootimg --kernel zImage --ramdisk ramdisk.cpio.gz --board smdk4x12 --base 0x10000000 --pagesize 2048 --ramdiskaddr 0x11000000 -o $KERNEL_PATH/boot.img

	# Copy boot.img
	cp boot.img $KERNEL_PATH/releasetools/$CUSTOM_PATH/zip
	cp boot.img $KERNEL_PATH/releasetools/$CUSTOM_PATH/tar
else
	cp zImage $KERNEL_PATH/releasetools/$CUSTOM_PATH/zip
	cp zImage $KERNEL_PATH/releasetools/$CUSTOM_PATH/tar
fi

# Creating flashable zip and tar
cd $KERNEL_PATH
cd releasetools/$CUSTOM_PATH/zip
zip -0 -r $version.zip * >/dev/null
mkdir -p $KERNEL_PATH/release
rm $KERNEL_PATH/release/* >/dev/null 2>&1
mv *.zip $KERNEL_PATH/release
cd ..

if [ "$TARGET" != "i9100" ] ; then
	cd tar
	tar cf $version.tar boot.img
	mv *.tar $KERNEL_PATH/release
fi
ls --color=auto -lh $KERNEL_PATH/release

# Cleanup
cd $KERNEL_PATH
rm $KERNEL_PATH/releasetools/$CUSTOM_PATH/zip/boot.img
rm $KERNEL_PATH/releasetools/$CUSTOM_PATH/tar/boot.img
rm $KERNEL_PATH/ramdisk.cpio.gz
rm $KERNEL_PATH/zImage
