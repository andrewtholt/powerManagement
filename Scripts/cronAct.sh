#!/bin/sh


PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
export PATH

. /opt/power/apps/penv.sh

if [ ! -f "/var/tmp/NOPOWER" ]; then
    sleep 40
    tsp -n fastAct
fi
