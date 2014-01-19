#!/sbin/busybox sh
BB="/sbin/busybox"

SEPARATOR() {
	echo "" >> $DEBUG_FILE;
	echo " ---------------------------- " >> $DEBUG_FILE;
	echo "" >> $DEBUG_FILE;
}

dmesg_log() {
	(# dmesg
	echo "dmesg-Info:" > $DEBUG_FILE;
	dmesg >> $DEBUG_FILE;
	SEPARATOR;
	echo "dmesg-Error:" >> $DEBUG_FILE;
	dmesg | grep -i "Error" >> $DEBUG_FILE;
	)&
}

logcat_log() {
	(# logcat
	echo "logcat-Info:" > $DEBUG_FILE_LOGCAT;
	system/bin/logcat -f $DEBUG_FILE_LOGCAT;
	wait 120;
	pkill logcat;
	)&
}


check_mount () {
for i in $($BB seq 1 5) ; do
if $BB test -d /sys/dev/block/179:13 ; then
break
else
echo "Waiting for internal mmc..."
echo $i;
$BB sleep 1
fi
done
}

$BB mount -t auto -o rw,seclabel,errors=continue,user_xattr,acl,barrier =1,data=ordered,noauto_da_alloc /dev/block/mmcblk0p14 /cache
if ! $BB grep -q /cache /proc/mounts ; then
$BB mount -t ext4 -o rw,seclabel,errors=continue,user_xattr,acl,barrier =1,data=ordered,noauto_da_alloc /dev/block/mmcblk0p14 /cache
$BB mount -t f2fs -o rw,seclabel,errors=continue,user_xattr,acl,barrier =1,data=ordered,noauto_da_alloc /dev/block/mmcblk0p14 /cache
fi
$BB date >/cache/mount_secondary.txt
exec >>/cache/mount_secondary.txt 2>&1

echo "first mount:"
$BB mount
echo ""

$BB mount -t auto -o rw,seclabel,errors=continue,user_xattr,acl,barrier =1,data=ordered,noauto_da_alloc /dev/block/mmcblk0p16 /.secondrom
if ! $BB grep -q /.secondrom /proc/mounts ; then
$BB mount -t ext4 -o rw,seclabel,errors=continue,user_xattr,acl,barrier =1,data=ordered,noauto_da_alloc /dev/block/mmcblk0p16 /.secondrom
$BB mount -t f2fs -o rw,seclabel,errors=continue,user_xattr,acl,barrier =1,data=ordered,noauto_da_alloc /dev/block/mmcblk0p16 /.secondrom
fi

#### system
$BB mkdir -p /system
$BB losetup /dev/block/loop0 /.secondrom/media/.secondrom/system.img
$BB mount -t auto -o ro /.secondrom/media/.secondrom/system.img /system
if ! $BB grep -q /system /proc/mounts ; then
$BB mount -t ext4 -o ro /.secondrom/media/.secondrom/system.img /system
$BB mount -t f2fs -o ro /.secondrom/media/.secondrom/system.img /system
fi

DEBUG_FILE=/cache/dmesg.txt
DEBUG_FILE_LOGCAT=/cache/logcat.txt

$BB cp /boot.txt /cache/boot.txt

#check_mount

#### data
$BB mkdir -p /.secondrom/media/.secondrom/data
$BB chmod 0771 /.secondrom/media/.secondrom/data
$BB chown system:system /.secondrom/media/.secondrom/data
$BB mount --bind /.secondrom/media/.secondrom/data /data
$BB mkdir -p /data/media
$BB mount --bind /.secondrom/media /data/media


layout_version=`$BB cat /data/.layout_version`
if $BB [ -d /data/media/0 ] && $BB [ "$layout_version" != 2 ] ; then
	echo "preparing layout_version"
	echo 2 > /data/.layout_version
cat /.secondrom/media/.secondrom/data/.layout_version
fi

echo "permissions of data/media"
$BB chown media_rw.media_rw /data/media
$BB chown -R media_rw.media_rw /data/media/*

ls -l /
echo ""
ls -l /.secondrom/media/.secondrom/data/

echo ""
$BB mount

logcat_log
dmesg_log

