#!/sbin/busybox sh
BB="/sbin/busybox"

$BB mkdir -p /.secondrom/media/.secondrom/data
$BB mount --bind /.secondrom/media/.secondrom/data /data
$BB mkdir /data/media
$BB mount --bind /.secondrom/media /data/media
