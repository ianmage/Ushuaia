#!/bin/sh
inFile=$1
len=$[${#inFile}-3]

if [ $len -le 0 ];then
echo "no file input"
exit
fi

if [[ ! -f $inFile || ! -r $inFile ]];then
echo "file not exist"
exit
fi

if [ ${inFile:$len} != ".sc" ];then
echo "in file not endswith .sc"
exit
fi

inName=`basename $inFile`
inDir=`dirname $inFile`

starts=${inName:0:3}
sType="unknown shader type"
if [ $starts == "vs_" ];then
sType=" --type vertex"
elif [ $starts == "fs_" ];then
sType=" --type fragment"
else
echo $sType
exit
fi

lenInName=${#inName}
varyName=${inName:3:$lenInName-6}
if [[ ${varyName:0-9} == "_instance" ]];then
varyName=${varyName:0:$lenInName-15}
fi
varyName=" --varyingdef "$inDir"/"$varyName".def"

outDir=$inDir"/../res/shaders/metal/"
if [ ! -d $outDir ];then
mkdir -p $outDir;
if [ ! -d $outDir ];then
echo "can not create ouput folder"
exit
fi
fi

outFile=" -o "$outDir${inFile:0:$len}".bin"
incl=" -i /Users/iAn/Programs/bgfx/examples/common -i /Users/iAn/Programs/bgfx/src"
platform=" --platform osx -p metal"

cmd="-f "$inFile$outFile$incl$sType$platform$varyName

defs=$2
if [ ${#defs} -ge 1 ];then
cmd=$cmd" --define "$defs
fi

#echo $cmd
~/Programs/bgfx_tools/shaderc $cmd