#!/bin/sh


PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
export PATH

. /opt/power/apps/penv.sh

LOGDIR=/opt/power/logs
LOGFILE=power.log

if [ ! -d $LOGDIR ]; then
	mkdir -p $LOGDIR
	touch ${LOGDIR}/${LOGFILE}
fi

if [ ! -f ${LOGDIR}/${LOGFILE} ]; then
	touch ${LOGDIR}/${LOGFILE}
fi

NOW=$(date)

printf "Discover:%s\n" "$NOW" >> ${LOGDIR}/${LOGFILE}
tsp -n discover -s 192.168.100.0/24 
tsp -n discover -s 192.168.0.0/24
echo "=======================" >> ${LOGDIR}/${LOGFILE}
