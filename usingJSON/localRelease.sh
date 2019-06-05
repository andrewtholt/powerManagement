#!/bin/bash

# set -x

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

BINS="syncMQTT pubSunRiset dispatch Server"

for b in $BINS; do
    echo $b
    rm -f $b
    cp ./Debug/src/${b} $TARGET/bin
done

# PYTHON="syncMQTT.py logger.py rmIO.py GUI.py"
PYTHON="logger.py rmIO.py GUI.py"

for p in $PYTHON; do
    echo $p
    rm -f $p
    cp ./Python/${p} $TARGET/bin
done

CFG="bridge.json"

for c in $CFG; do
    echo $c
    cp ./JSON/${c} $TARGET/etc
done
