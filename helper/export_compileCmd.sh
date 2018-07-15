#!/bin/sh

path=$0
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ${path%/*}/../build