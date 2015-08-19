#!/bin/bash

# set -x

echo $EUID

if [ $EUID -ne 0 ]; then 
    echo "Must be root, sorry."
    exit 1
fi

BASE=/srv/ftp/manage

if [ ! -d $BASE ]; then
    mkdir -p $BASE
    mkdir -p $BASE/Data
    mkdir -p $BASE/Scripts
fi

cp ../Data/*.ini $BASE/Data

SCRIPTS="debianZram.sh inifile.lua node.lua"

for N in $SCRIPTS; do
    cp $N $BASE/Scripts
done
