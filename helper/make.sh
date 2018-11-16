#!/bin/sh

cwd=$(pwd)
fDir=$(dirname $0)
cd ${fDir}/..
rm -f CMakeCache.txt
rm -rf CMakeFiles
if [ "$1" = "gmake" ];then
cmake -G "Unix Makefiles"
elif [ "$1" = "xcode" ];then
cmake -G "Xcode"
else
echo UnKnown arg1, should be gmake/xcode
fi