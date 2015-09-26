#!/bin/bash

set -e

count=0
for i in $( ls /data/output/*.opus ); do
  echo $count perl wav2speechtext.pl $i
  perl opus2speechtext.pl $i &
  if [ $((count % 20)) -eq 0 ]
    then
      echo waiting
      wait
  fi
  count=$((count+1))
done

wait

