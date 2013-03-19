#!/sbin/busybox sh
BB="/sbin/busybox"

$BB mv -f /cache/data.txt /cache/data_old.txt
$BB date >>/cache/data.txt
exec >>/cache/data.txt 2>&1

#move .secondrom folder back to the original location if cm10.1 moved it to a subfolder
if $BB [ -d /.secondrom/media/0/.secondrom ];then
  if $BB [ ! -d /.secondrom/media/.secondrom ];then
    $BB mkdir /.secondrom/media/.secondrom
    $BB mv -f /.secondrom/media/0/.secondrom/* /.secondrom/media/.secondrom
    $BB rmdir /.secondrom/media/0/.secondrom
  fi
fi

$BB mount -t tmpfs tmpfs /system/lib/modules
$BB ln -s /lib/modules/* /system/lib/modules

$BB mkdir -p /.secondrom/media/.secondrom/data
$BB mount --bind /.secondrom/media/.secondrom/data /data
$BB mkdir /data/media
$BB mount --bind /.secondrom/media /data/media
