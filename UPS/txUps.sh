#!/bin/sh

# set -x

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin
export PATH

GROUP="POWER"
USER="UPS"
SERVER="beaglexm"

upsAgent -n $SERVER | lightSource -g $GROUP -u $USER

