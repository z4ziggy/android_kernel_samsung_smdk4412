#!/bin/bash


build ()
{
    local target=$1
    ./build_combo.sh $target
}
    
targets=("$@")
if [ 0 = "${#targets[@]}" ] ; then
    targets=(t0lte n7100 i9300)
fi

START=$(date +%s)

for target in "${targets[@]}" ; do 
    build $target
done

END=$(date +%s)
ELAPSED=$((END - START))
E_MIN=$((ELAPSED / 60))
E_SEC=$((ELAPSED - E_MIN * 60))
printf "Elapsed: "
[ $E_MIN != 0 ] && printf "%d min(s) " $E_MIN
printf "%d sec(s)\n" $E_SEC
