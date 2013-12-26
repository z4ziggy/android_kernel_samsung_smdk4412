#!/sbin/sh
#
#
#

MOUNTPOINT=$1
if [ -f "$MOUNTPOINT"dualboot/boot.img ] ; then
   if $BB [ ! -d /sys/dev/block/259:0 ] ; then
	cp -f "$MOUNTPOINT"dualboot/boot.img /dev/block/mmcblk0p5 || exit 1
   else
	cp -f "$MOUNTPOINT"dualboot/boot.img /dev/block/mmcblk0p8 || exit 1
   fi
else
exit 1
fi

exit 0
