#!/bin/bash

# set -x

USER="automation"
PASS="automation"
DBNAME="automation"

DATE=$(date "+%Y%m%d")

DIR_NAME="$HOME/Backup"

if [ ! -d $DIR_NAME ]; then
    mkdir $DIR_NAME
fi

NUM=1

FNAME=${DIR_NAME}/data_${DATE}.sql

DONE="FALSE"

if [ ! -f $FNAME ]; then
    DONE="TRUE"
fi


while [ "$DONE" == "FALSE" ]; do
    FNAME=${DIR_NAME}/data_${DATE}_${NUM}.sql

    if [ ! -f $FNAME ]; then
        DONE="TRUE"
    else
        let "NUM=NUM+1"
    fi

done

echo "Backup to $FNAME"
mysqldump -t -u $USER -p${PASS} -t $DBNAME  > $FNAME
exit 0
