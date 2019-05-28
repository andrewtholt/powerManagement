#!/bin/bash

set -x

TARGET=./output
if [ ! -d $TARGET ]; then
    mkdir -p $TARGET/
fi

mkdir -p $TARGET/bin
mkdir -p $TARGET/etc

if [ ! -d ./Debug ]; then
    echo "Nothing to install"
    exit 1
fi

BINS="dispatch Server"

for b in $BINS; do
    echo $b
    cp ./Debug/src/${b} $TARGET/bin
done

PYTHON="syncMQTT.py"

for p in $PYTHON; do
    echo $p
    cp ./Python/${p} $TARGET/bin
done

CFG="bridge.json"

for c in $CFG; do
    echo $c
    cp ./JSON/${c} $TARGET/etc
done