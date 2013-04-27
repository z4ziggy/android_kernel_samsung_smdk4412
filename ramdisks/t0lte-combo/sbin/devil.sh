#!/sbin/busybox sh

# Logging
/sbin/busybox cp /data/user.log /data/user.log.bak
/sbin/busybox rm /data/user.log
exec >>/data/user.log
exec 2>&1

BB="/sbin/busybox";

mount -o remount,rw /system
$BB mount -t rootfs -o remount,rw rootfs

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

if [ ! -d /data/.devil ]; then
$BB mkdir -p /data/.devil;
fi;

# reset config-backup-restore
if [ -f /data/.devil/restore_running ]; then
rm -f /data/.devil/restore_running;
fi;

# for dev testing
PROFILES=`$BB ls -A1 /data/.devil/*.profile`;
for p in $PROFILES; do
cp $p $p.test;
done;

CONFIG_XML=/res/customconfig/customconfig.xml;
if [ ! -f $CONFIG_XML ]; then
mount -o remount,rw /;
  . /res/customconfig/customconfig.xml.generate > $CONFIG_XML;
fi;


. /res/customconfig/customconfig-helper

[ ! -f /data/.devil/default.profile ] && cp /res/customconfig/default.profile /data/.devil;

$BB chmod 0777 /data/.devil/ -R;

read_defaults;
read_config;

if [ "$logger" == "on" ];then
insmod /lib/modules/logger.ko
fi

# disable debugging on some modules
if [ "$logger" == "off" ];then
  rm -rf /dev/log
  echo 0 > /sys/module/ump/parameters/ump_debug_level;
  echo 0 > /sys/module/mali/parameters/mali_debug_level;
  echo 0 > /sys/module/kernel/parameters/initcall_debug;
  echo 0 > /sys//module/lowmemorykiller/parameters/debug_level;
  echo 0 > /sys/module/earlysuspend/parameters/debug_mask;
  echo 0 > /sys/module/alarm/parameters/debug_mask;
  echo 0 > /sys/module/alarm_dev/parameters/debug_mask;
  echo 0 > /sys/module/binder/parameters/debug_mask;
  echo 0 > /sys/module/xt_qtaguid/parameters/debug_mask;
fi

######################################
# Loading Modules
######################################
$BB chmod -R 755 /lib;


#if [ "$logger" == "off" ];then
#rmmod /lib/modules/logger.ko
#fi


#(
#        $BB sh /sbin/ext/smoothlauncher.sh &
#)&

# some nice thing for dev
$BB ln -s /sys/devices/system/cpu/cpu0/cpufreq /cpufreq;
$BB ln -s /sys/devices/system/cpu/cpufreq/ /cpugov;

# enable kmem interface for everyone by GM
echo "0" > /proc/sys/kernel/kptr_restrict;

(
	echo 0 > /tmp/uci_done;
	chmod 666 /tmp/uci_done;
	# custom boot booster
	while [ "`cat /tmp/uci_done`" != "1" ]; do
		if [ "$scaling_max_freq" != "1500000" ] || [ "$scaling_max_freq" != "1600000"]; then
		echo "1400000" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq;
		echo "1400000" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq;
		pkill -f "com.gokhanmoral.stweaks.app";
		echo "Waiting For UCI to finish";
		sleep 20;
		fi
	done;

	# restore normal freq.
	echo "$scaling_min_freq" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq;
	echo "$scaling_max_freq" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq;
)&

# Stop uci.sh from running all the PUSH Buttons in stweaks on boot.
$BB mount -o remount,rw rootfs;
$BB chown root:system /res/customconfig/actions/ -R;
$BB chmod 6755 /res/customconfig/actions/*;
$BB chmod 6755 /res/customconfig/actions/push-actions/*;
$BB mv /res/customconfig/actions/push-actions/* /res/no-push-on-boot/;

# some initialization code
ccxmlsum=`md5sum $CONFIG_XML | awk '{print $1}'`
if [ "a${ccxmlsum}" != "a`cat /data/.devil/.ccxmlsum`" ];
then
#  rm -f /data/.devil/*.profile
  echo ${ccxmlsum} > /data/.devil/.ccxmlsum;
fi

# apply STweaks settings
echo "booting" > /data/.devil/booting;
pkill -f "com.gokhanmoral.stweaks.app";
# apply STweaks defaults
export CONFIG_BOOTING=1
nohup $BB sh /res/uci.sh restore;
export CONFIG_BOOTING=
echo "1" > /tmp/uci_done;

# restore all the PUSH Button Actions back to there location
$BB mount -o remount,rw rootfs;
$BB mv /res/no-push-on-boot/* /res/customconfig/actions/push-actions/;
pkill -f "com.gokhanmoral.stweaks.app";
$BB rm -f /data/.devil/booting;

# ==============================================================
# STWEAKS FIXING
# ==============================================================
# change USB mode MTP or Mass Storage
$BB sh /res/uci.sh usb-mode ${usb_mode};

$BB mount -t rootfs -o remount,ro rootfs
$BB mount -o remount,ro /system
