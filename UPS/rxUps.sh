#!/bin/sh

# set -x

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin
export PATH

GROUP="POWER"
USER="RX"
SERVER="beaglexm"

lightSink -l -g $GROUP -u $USER

