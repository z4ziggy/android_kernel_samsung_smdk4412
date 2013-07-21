#!/sbin/sh
#
#
#

MOUNTPOINT=$1
FILE=$2
LOCATION=$3

updater_script_path="META-INF/com/google/android/updater-script"
echo "mountpoint: $MOUNTPOINT"
echo "file: $FILE"
echo "location: $LOCATION"
rm -rf $MOUNTPOINT/dualboot/*
mkdir -p $MOUNTPOINT/dualboot || exit 1

if [ ! -s $FILE ] ; then
echo "could not find the file! Are there spaces in the name or path?"
exit 2
fi

############################## primary rom ####################################################
if [ "$LOCATION" == "primaryrom" ] ; then
#### unzip META-INF

# get the kernel
dd if=/dev/block/mmcblk0p8 of="$MOUNTPOINT"dualboot/boot.img || exit 1

echo "installing rom now ..."
cd /


####################################### secondary rom ##################################################
elif [ "$LOCATION" == "secondaryrom" ] ; then

unzip_binary -o $FILE $updater_script_path -d "$MOUNTPOINT"dualboot || exit 1
#### use mount script ####
if $BB [ ! -d /sys/dev/block/259:0 ] ; then

	sed 's|mount("ext4", "EMMC", "/dev/block/mmcblk0p9", "/system");|run_program("/sbin/system_mount.sh", "secondary");|g' -i "$MOUNTPOINT"dualboot/$updater_script_path || exit 1

### also use script for formating ###
	sed 's|format("ext4", "EMMC", "/dev/block/mmcblk0p9", "0", "/system");|run_program("/sbin/system_format.sh", "secondary");|g' -i "$MOUNTPOINT"dualboot/$updater_script_path || exit 1

	sed 's|format("ext4", "EMMC", "/dev/block/mmcblk0p9");|run_program("/sbin/system_format.sh", "secondary");|g' -i "$MOUNTPOINT"dualboot/$updater_script_path || exit 1

else

	sed 's|mount("ext4", "EMMC", "/dev/block/mmcblk0p13", "/system");|run_program("/sbin/system_mount.sh", "secondary");|g' -i "$MOUNTPOINT"dualboot/$updater_script_path || exit 1

### also use script for formating ###
	sed 's|format("ext4", "EMMC", "/dev/block/mmcblk0p13", "0", "/system");|run_program("/sbin/system_format.sh", "secondary");|g' -i "$MOUNTPOINT"dualboot/$updater_script_path || exit 1

	sed 's|format("ext4", "EMMC", "/dev/block/mmcblk0p13");|run_program("/sbin/system_format.sh", "secondary");|g' -i "$MOUNTPOINT"dualboot/$updater_script_path || exit 1

fi

# get the kernel
if $BB [ ! -d /sys/dev/block/259:0 ] ; then
dd if=/dev/block/mmcblk0p5 of="$MOUNTPOINT"dualboot/boot.img || exit 1
else
dd if=/dev/block/mmcblk0p8 of="$MOUNTPOINT"dualboot/boot.img || exit 1
fi

cd $MOUNTPOINT/dualboot
zip $FILE $updater_script_path
echo "installing rom now ..."
cd /
fi

umount -f system
exit 0
