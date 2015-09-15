#!/bin/bash

for i in $( ls /data/output/*.srt ); do
  echo perl srt2srtproto.pl $i
  perl srt2srtproto.pl < $i > ${i}proto
done
