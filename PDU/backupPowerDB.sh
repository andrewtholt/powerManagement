#!/bin/sh

# set -x 
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PATH

. /opt/power/apps/penv.sh 

TODAY=$(date '+%j')
DIR=$HOME/Backup
FILE=$DIR/powerDB_${TODAY}.sql

if [ ! -d $DIR ]; then
    mkdir -p $DIR
fi

if [ -f $FILE ]; then
    echo "Backup for today done."
else
    echo "Start Backup ..."
    echo ".dump" | sqlite3 $POWER_DB > $FILE
    echo "... done."
fi

exit 0

