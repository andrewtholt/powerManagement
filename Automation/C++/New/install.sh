#!/bin/bash

set -x 

export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

DEST="/usr/local/Scripts"

sudo mkdir -p $DEST
sudo cp pubDay.sh $DEST
sudo cp pubTime.sh $DEST

