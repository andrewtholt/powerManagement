#!/bin/sh

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

. /opt/power/apps/penv.sh 

tsp -n backupPowerDB.sh

