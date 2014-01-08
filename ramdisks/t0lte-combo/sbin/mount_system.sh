#!/sbin/busybox sh
BB="/sbin/busybox"
$BB mount -t ext4 -o rw /dev/block/mmcblk0p14 /cache
$BB mount -t f2fs -o rw /dev/block/mmcblk0p14 /cache
$BB date >/cache/system_mount.txt
exec >>/cache/system_mount.txt 2>&1

echo "first mount:"
$BB mount
echo ""

$BB ls -l /.secondrom
echo ""

$BB ls -l /.secondrom
echo ""
$BB mount -t ext4 -o rw /dev/block/mmcblk0p16 /.secondrom
$BB mount -t f2fs -o rw /dev/block/mmcblk0p16 /.secondrom

#### system
$BB mkdir -p /system
$BB losetup /dev/block/loop0 /.secondrom/media/.secondrom/system.img
$BB mount -t ext4 -o ro /.secondrom/media/.secondrom/system.img /system
$BB mount -t f2fs -o ro /.secondrom/media/.secondrom/system.img /system

echo ""
$BB mount

