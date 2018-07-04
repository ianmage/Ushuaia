#!/bin/sh

for file in `find . -name "*.sc"`
do
	if test -f $file
	then
		. ./fxc.sh $file
	fi
done