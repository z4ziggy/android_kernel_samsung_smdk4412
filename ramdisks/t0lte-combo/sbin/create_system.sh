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

   if ! $BB grep -q /.secondrom /proc/mounts ; then
	$MOUNT -t ext4 -o rw /dev/block/$BLOCKDEVICE /.secondrom
   fi

   if ! $BB grep -q /data/media /proc/mounts ; then
	$BB mkdir -p /data/media
	$MOUNT --bind /.secondrom/media /data/media
   fi

$BB mkdir -p /.secondrom/media/.secondrom/data
$BB mkdir -p /.secondrom/media/.secondrom
system=/.secondrom/media/.secondrom/system.img

if $BB [ ! -f $system ] ; then
	# create a file 1.5Gb
	$BB dd if=/dev/zero of=$system bs=1024 count=1572864 || exit 1
	# create ext4 filesystem
	$BB mke2fs -F -T ext4 $system || exit 1
fi

exit 0
