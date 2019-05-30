#!/bin/bash

declare -a files
files=$(cat ff.txt)

for f in ${files[@]}
do
   echo updating $f
   sed s/private/'hestia\/private'/g $f > temp
   mv temp $f
done 

rm temp
