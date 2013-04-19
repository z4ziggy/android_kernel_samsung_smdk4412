#!/sbin/busybox sh

$BB date >>modules.txt
exec >>modules.txt 2>&1

mount -t tmpfs tmpfs /system/lib/modules
ln -s /lib/modules/* /system/lib/modules

# ko files for exfat
    insmod /system/lib/modules/exfat_core.ko
    insmod /system/lib/modules/exfat_fs.ko
