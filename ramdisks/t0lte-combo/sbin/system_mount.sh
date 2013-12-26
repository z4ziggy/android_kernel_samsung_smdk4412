#!/sbin/sh

BB="busybox"
FILESYSTEM=$1

if $BB [ ! -d /sys/dev/block/259:0 ] ; then
  BLOCKDEVICE=mmcblk0p12
elif $BB [ -d /sys/dev/block/259:1 ] ; then
  BLOCKDEVICE=mmcblk0p17
else
  BLOCKDEVICE=mmcblk0p16
fi

if $BB [ "$FILESYSTEM" == "secondary" ]; then
$BB mkdir -p /.secondrom
$BB mount -t ext4 /dev/block/$BLOCKDEVICE /.secondrom
$BB mount -t ext4 -o rw /.secondrom/media/.secondrom/system.img /system
elif $BB [ "$FILESYSTEM" == "primary" ] ; then
   if $BB [ ! -d /sys/dev/block/259:0 ] ; then
	$BB mount -t ext4 -o rw /dev/block/mmcblk0p9 /system
   else
	$BB mount -t ext4 -o rw /dev/block/mmcblk0p13 /system
   fi
fi
