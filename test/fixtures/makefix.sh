#!/bin/bash
set -e
TMP=$(mktemp -d)
DIR="$( cd "$( dirname "$0" )" && pwd )"
#define if we use libav or ffmpeg
echo "Creating Video for Google Tests..."
for i in {1..20}
do
	((y=$i*10))
	((u=210-$i*10))
	printf -v name "frame-%02d" $i
  convert -size 18x19 xc:"yuv($y,128,128)" -depth 8 -sampling-factor 4:2:2 $TMP/$name.jpg
done

OPTS="-r 25 -y"
#$FFMPEG $OPTS -i "$TMP/frame-%02d.jpg" -g 1 -keyint_min 0 -pix_fmt yuv420p -c:v mpeg4 $DIR/goutput.mp4
$DIR/../video-converter "$TMP/frame-%02d.jpg" "$DIR/goutput.mov"
rm -rf $TMP/*

echo "Creating Video for Visual Tests..."

for i in {1..80}
do
	((x=$i*24))
	printf -v name "frame-%02d" $i
  convert -size 1920x1800 xc:"rgb(0,0,0)" -fill "rgb(50,50,50)" -draw "rectangle 0,0,$x,1080" -depth 8 -sampling-factor 4:2:2 $TMP/$name.jpg
done

$DIR/../video-converter "$TMP/frame-%02d.jpg" "$DIR/voutput.mov"

rm -rf $TMP/*

if [ ! -f "$DIR/input_nasa_launch.mov" ] ;then
	echo "creating real world video test"
	wget -O "$DIR/input_nasa_launch.mov" http://s3.amazonaws.com/akamai.netstorage/qt.nasa-global/ksc/ksc_102407_sts120_launch_1080i.mov

fi

rm -rf $TMP
$DIR/../video-converter "$DIR/input_nasa_launch.mov" "$DIR/real_test.mov"
