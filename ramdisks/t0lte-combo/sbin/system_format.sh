#!/sbin/sh

BB="busybox"
FILESYSTEM=$1

$BB date >>/tmp/system_format.txt
exec >>/tmp/system_format.txt 2>&1

if $BB [ "$FILESYSTEM" == "secondary" ]; then
$BB umount -f /system
$BB mkdir -p /system
system=/.secondrom/media/.secondrom/system.img
$BB mount -t ext4 -o rw $system /system
$BB rm -rf /system/*
$BB rm -rf /system/.*
$BB mke2fs -F -T ext4 $system || exit 1
else
exit 1
fi
exit 0
