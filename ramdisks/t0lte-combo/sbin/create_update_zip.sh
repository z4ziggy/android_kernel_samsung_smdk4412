#!/sbin/sh
filename=$(basename $1)
mkdir -p $1/META-INF/com/google/android/
# copy meta files to pwd
cp /res/image-binary $1/META-INF/com/google/android/update-binary | exit 1
cp /res/image-edify $1/ | exit 1

# go to pwd
cd $1

mkdir -p ../customzip

# create custom rom
zip -r ../customzip/$filename.zip *
cd ..
rm -rf $1
