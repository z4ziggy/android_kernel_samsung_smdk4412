#!/sbin/busybox sh
BB="/sbin/busybox"
$BB date >/cache/mount.txt
exec >>/cache/mount.txt 2>&1

DEBUG_FILE=/cache/dmesg.txt
DEBUG_FILE_LOGCAT=/cache/logcat.txt

$BB cp /boot.txt /cache/boot.txt

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

echo "first mount:"
$BB mount
echo ""

#check_mount
#/dev/block/mmcblk0p13    /system           ext4      ro                                                                                 wait
$BB mount -t ext4 /dev/block/mmcblk0p13 /system
$BB mount -t f2fs /dev/block/mmcblk0p13 /system



#/dev/block/mmcblk0p12    /cache            ext4      noatime,nosuid,nodev,journal_async_commit,errors=panic                             wait,check
$BB mount -t ext4 -o rw /dev/block/mmcblk0p12 /cache
$BB mount -t f2fs -o rw /dev/block/mmcblk0p12 /cache

#/dev/block/mmcblk0p16    /data             ext4      noatime,nosuid,nodev,discard,noauto_da_alloc,journal_async_commit,errors=panic     wait,check,encryptable=footer
$BB mount -t ext4 -o rw /dev/block/mmcblk0p16 /data
$BB mount -t f2fs -o rw /dev/block/mmcblk0p16 /data

echo ""
$BB mount

logcat_log
dmesg_log

