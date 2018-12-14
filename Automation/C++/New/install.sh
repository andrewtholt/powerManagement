#!/bin/bash

set -x

RELEASE="0.1"
LIB_DEST="/usr/local/lib"
INC_DEST="/usr/local/include"

sudo cp ./libplc.so $LIB_DEST
HERE=$(pwd)
cd $LIB_DEST

if [ -L ./libplc.so.${RELEASE} ]; then
    echo "./libplc.so.${RELEASE} already exists"
    exit 1
else
    sudo ln -s ./libplc.so ./libplc.so.${RELEASE}
fi

cd $HERE
sudo cp plcMQTT.h $INC_DEST
sudo cp plcBase.h $INC_DEST

