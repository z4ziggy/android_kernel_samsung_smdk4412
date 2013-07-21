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
mount -t ext4 /dev/block/$BLOCKDEVICE /.secondrom || exit 1
fi

if ! $BB grep -q /data /proc/mounts ; then
   if $BB grep -q secondary /etc/.firstrundone; then # secondary
	mkdir -p /.secondrom/media/.secondrom/data
	mkdir -p /data
	mount --bind /.secondrom/media/.secondrom/data /data

   elif $BB grep -q primary /etc/.firstrundone; then # primary
	mkdir -p /data
	mount --bind /.secondrom /data
   fi
fi

if ! $BB grep -q /data/media /proc/mounts ; then
mkdir -p /data/media
mount --bind /.secondrom/media /data/media || exit 1
fi

exit 0


