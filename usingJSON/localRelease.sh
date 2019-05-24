#!/bin/bash

set -x

TARGET=./output
if [ ! -d $TARGET ]; then
    mkdir -p $TARGET
fi
