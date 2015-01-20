#!/bin/bash

for i in `ls *.wav`
do
echo $i
echo $i"_1600.wav"
    sox -v 0.5 $i -r 16000 $i"_1600.wav"
done
