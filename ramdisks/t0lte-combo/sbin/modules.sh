#!/sbin/busybox sh
BB=/sbin/busybox

$BB date >>modules.txt
exec >>modules.txt 2>&1

#  try ko files for exfat

if [ -f /system/lib/modules/exfat_core.ko ] ; then
    insmod /system/lib/modules/exfat_core.ko
    insmod /system/lib/modules/exfat_fs.ko
fi

if [ -f /lib/modules/exfat_core.ko ] ; then
    insmod /lib/modules/exfat_core.ko
    insmod /lib/modules/exfat_fs.ko
fi

# usb connection workaround, neeed because of broken default.prop execution

if [ -f /data/property/persist.sys.usb.config ] ; then
	if grep -q mtp /data/property/persist.sys.usb.config; then
		echo "mtp should be working already"
	else
		setprop persist.sys.usb.config mtp,adb
	fi
else
	setprop persist.sys.usb.config mtp,adb
fi 
