#!/bin/sh


PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
export PATH

. /opt/power/apps/penv.sh

if [ ! -f "/var/tmp/NOPOWER" ]; then
    tsp -n update
fi
