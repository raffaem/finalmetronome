#!/bin/sh

cd ./midi_drumset

for file in *;
do
	echo "Operating on "$file
	 gst-launch filesrc location=$file ! timidity ! audioconvert ! vorbisenc bitrate=250000 ! oggmux ! filesink location=../ogg_drumset/$file.ogg
done

