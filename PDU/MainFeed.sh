#!/bin/sh
# 
# Turn on the main feed.
# and wait until it is on.
#
# set -x

. /opt/power/apps/penv.sh

exitFlag=0
DEVICE="MainFeed"
SENT=0

while [ $exitFlag -eq 0 ] ; do
    printf "Round...\n"
    power qstatus $DEVICE

    if [ $? -eq 1 ]; then
        printf "Its on\n"
        lock $DEVICE
        exitFlag=1
    else
        printf "Switch it on\n"
        if [ $SENT -eq 0 ]; then
            unlock $DEVICE
            power on $DEVICE
            SENT=1
        fi

        sleep 10
    fi
done
