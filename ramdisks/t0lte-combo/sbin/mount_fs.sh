#!/sbin/sh

#
#
# mount filesystem

BB="busybox"
MOUNT="busybox mount"
UMOUNT="busybox umount -f"

$BB date >>/tmp/mount_fs.txt
exec >>/tmp/mount_fs.txt 2>&1

$UMOUNT /system
$UMOUNT /data
if $BB [ ! -d /.secondrom ] ; then
$BB mkdir -p /.secondrom
fi

if $BB [ ! -d /sys/dev/block/259:0 ] ; then
  BLOCKDEVICE=mmcblk0p12
elif $BB [ -d /sys/dev/block/259:1 ] ; then
  BLOCKDEVICE=mmcblk0p17
else
  BLOCKDEVICE=mmcblk0p16
fi

########## initial preparation of dualboot recovery ##############################################
if [ "$1" == "initial" ] ; then
   if $BB [ ! -f /etc/.firstrundone ] ; then
   	$BB touch /etc/.firstrundone
   	$BB cp /etc/default.fstab /etc/recovery.fstab
   	$BB killall recovery
   fi

   if ! $BB grep -q /.secondrom /proc/mounts ; then
	$MOUNT -t ext4 -o rw /dev/block/$BLOCKDEVICE /.secondrom
   fi
$BB mkdir -p /.secondrom/media/.secondrom/data
   $UMOUNT /data/media
   if ! $BB grep -q /data/media /proc/mounts ; then
	$BB mkdir -p /data/media
	$MOUNT --bind /.secondrom/media /data/media
   fi

########## if called with umount parameter, just umount everything and exit ######################
elif [ "$1" == "umount" ] ; then
   $BB cp /etc/fstab.default /etc/fstab
   if $BB grep -q "/system" /proc/mounts ||
	$BB grep -q "/data" /proc/mounts ; then
		exit 1
   fi

elif [ "$1" == "boot_primary" ] ; then
   $MOUNT -t ext4 -o rw /dev/block/$BLOCKDEVICE /.secondrom
   $BB echo 0 > /.secondrom/.secondaryboot

elif [ "$1" == "boot_secondary" ] ; then
   $MOUNT -t ext4 -o rw /dev/block/$BLOCKDEVICE /.secondrom
   $BB echo 1 > /.secondrom/.secondaryboot

elif [ "$1" == "primary_unsecure" ] ; then
   $BB cp /etc/primary.fstab /etc/recovery.fstab
   $BB echo primary > /etc/.firstrundone
   $BB killall recovery

elif [ "$1" == "secondary_unsecure" ] ; then
   $BB cp /etc/secondary.fstab /etc/recovery.fstab
   $BB echo secondary > /etc/.firstrundone
   $BB killall recovery

elif [ "$1" == "primary" ] ; then
   if $BB grep -q "/system" /proc/mounts ||
	$BB grep -q "/data" /proc/mounts ; then
	$UMOUNT /system
	$UMOUNT /data
   fi

	$BB cp /etc/primary.fstab /etc/fstab
   if ! $BB grep -q /.secondrom /proc/mounts ; then
	$MOUNT -t ext4 -o rw /dev/block/$BLOCKDEVICE /.secondrom
   fi
	$MOUNT --bind /.secondrom /data
   	if ! $BB grep -q /data/media /proc/mounts ; then
	$BB mkdir -p /data/media
	$MOUNT --bind /.secondrom/media /data/media
   	fi
	if $BB [ ! -d /sys/dev/block/259:0 ] ; then
	$BB mount -t ext4 -o rw /dev/block/mmcblk0p9 /system
	$MOUNT -t ext4 -o rw /dev/block/mmcblk0p8 /cache
   	else
	$BB mount -t ext4 -o rw /dev/block/mmcblk0p13 /system
	$MOUNT -t ext4 -o rw /dev/block/mmcblk0p12 /cache
   	fi	
elif [ "$1" == "secondary" ] ; then
   if $BB grep -q "/system" /proc/mounts ||
	$BB grep -q "/data" /proc/mounts ; then
	$UMOUNT /system
	$UMOUNT /data
   fi
	$BB cp /etc/secondary.fstab /etc/fstab
   if ! $BB grep -q /.secondrom /proc/mounts ; then
	$MOUNT -t ext4 -o rw /dev/block/$BLOCKDEVICE /.secondrom
   fi
	$BB mkdir -p /.secondrom/media/.secondrom/data
	$MOUNT --bind /.secondrom/media/.secondrom/data /data
   	if ! $BB grep -q /data/media /proc/mounts ; then
	$BB mkdir -p /data/media
	$MOUNT --bind /.secondrom/media /data/media
   	fi
	$MOUNT -t ext4 -o rw /.secondrom/media/.secondrom/system.img /system
	if $BB [ ! -d /sys/dev/block/259:0 ] ; then
	$MOUNT -t ext4 -o rw /dev/block/mmcblk0p10 /cache
	else
	$MOUNT -t ext4 -o rw /dev/block/mmcblk0p14 /cache
	fi
else
	echo "missing paramter"
	exit 1
fi

exit 0
