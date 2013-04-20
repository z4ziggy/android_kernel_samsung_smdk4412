#!/sbin/busybox sh

mount -o remount,rw /system
/sbin/busybox mount -t rootfs -o remount,rw rootfs

mkswap /dev/block/zram0
swapon /dev/block/zram0
echo 90 > /proc/sys/vm/swappiness

echo -1 > /sys/devices/system/gpu/time_in_state

for i in /sys/block/*/queue/add_random;do echo 0 > $i;done

echo 0 > /proc/sys/kernel/randomize_va_space

echo 3 > /sys/module/cpuidle_exynos4/parameters/enable_mask

if [ ! -f /system/app/STweaks.apk ]; then
  cat /res/STweaks.apk > /system/app/STweaks.apk
  chown 0.0 /system/app/STweaks.apk
  chmod 644 /system/app/STweaks.apk
fi

mkdir -p /mnt/ntfs
chmod 777 /mnt/ntfs
mount -o mode=0777,gid=1000 -t tmpfs tmpfs /mnt/ntfs

/res/uci.sh apply


/sbin/busybox mount -t rootfs -o remount,ro rootfs
mount -o remount,ro /system
