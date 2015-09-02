#!/bin/sh

echo "Checking dependincies"

DB=sqlite3
LANG=python

if [ `which $DB` ]; then
    echo "$DB found"
else
    echo "Can't find $DB"
    exit -1
fi

if [ `which $LANG` ]; then
    echo "$LANG found"
else
    echo "Can't find $LANG"
    exit -1
fi

if [ $(grep ^power: /etc/group ) ]; then
    echo "Power group exists."
else
    echo "Power group DOES NOT exist, creating ..."
    addgroup power
    echo "... done."
fi

DESTDIR="/opt/power"
echo "This defines the point below which, by default, software and data"
echo "will be installed."
printf "Destination [%s]:" $DESTDIR
read TMP

if [ ! -z $TMP ]; then
    DESTDIR=tmp
fi

PDIR="${DESTDIR}"
export PDIR

DATA_DIR=${PDIR}/data
APPS_DIR=${PDIR}/apps

printf "Database    [%s]:" $DATA_DIR
read TMP

if [ ! -z $TMP ]; then
    DATA_DIR=$TMP
fi

POWER_DB="${DATA_DIR}/power.db"
export POWER_DB

if [ ! -d $PDIR ]; then
    echo "Creating $PDIR"
    mkdir -p $PDIR
fi

chgrp power $PDIR
chmod 775 $PDIR

if [ ! -d $DATA_DIR ]; then
    echo "Creating $DATA_DIR"
    mkdir -p $DATA_DIR
fi

chgrp power $DATA_DIR
chmod 775 $DATA_DIR

if [ ! -d $APPS_DIR ]; then
    echo "Creating $APPS_DIR"
    mkdir -p $APPS_DIR
fi

chgrp power $APPS_DIR
chmod 775 $APPS_DIR

echo "Copying default data ..."
cp ./data/*.txt $DATA_DIR
cp setup.sql $DATA_DIR
echo "... done."

echo "Creating database ..."
HERE=`pwd`
# cd $DATA_DIR
# $DB power.db < setup.sql

./start -c

chgrp power $POWER_DB
chmod 775 $POWER_DB

cd $HERE
echo "... done"

echo "Copying apps ...."
cp ./fastAct.py $APPS_DIR
cp ./cancel.py $APPS_DIR
cp ./discover.py $APPS_DIR
cp ./eventAction.py $APPS_DIR
cp ./exists.py $APPS_DIR
cp ./fail.py $APPS_DIR
cp ./host.py $APPS_DIR
cp ./locking.py $APPS_DIR
cp ./onPowerFailure.py $APPS_DIR
cp ./onPowerRestore.py $APPS_DIR
cp ./pending.py $APPS_DIR
cp ./power.py $APPS_DIR
cp ./powerCheck.py $APPS_DIR
cp ./start.py $APPS_DIR
cp ./update.py $APPS_DIR
cp ./backupPowerDB.sh $APPS_DIR

cd $APPS_DIR
ln -fs ./fastAct.py ./fastAct
ln -fs ./cancel.py ./cancel
ln -fs ./discover.py ./discover
ln -fs ./eventAction.py ./eventAction
ln -fs ./exists.py ./exists
ln -fs ./fail.py ./fail
ln -fs ./host.py ./host
ln -fs ./locking.py ./lock
ln -fs ./locking.py ./unlock
ln -fs ./onPowerFailure.py onPowerFailure
ln -fs ./onPowerRestore.py onPowerRestore
ln -fs ./pending.py ./pending
ln -fs ./power.py ./power
ln -fs ./powerCheck.py ./powerCheck
ln -fs ./start.py ./start
ln -fs ./update.py ./update
cd $HERE
echo "... done."


printf "POWER_DB=%s\n" $POWER_DB > ${APPS_DIR}/penv.sh
printf "export POWER_DB\n\n" >> ${APPS_DIR}/penv.sh

printf "PDIR=%s\n" $PDIR >> ${APPS_DIR}/penv.sh
printf "export PDIR\n\n" >> ${APPS_DIR}/penv.sh

printf "PATH=\${PATH}:%s\n" $APPS_DIR >> ${APPS_DIR}/penv.sh
printf "export PATH\n\n" >> ${APPS_DIR}/penv.sh

echo "Add the following to your environment."
echo "Or source ${APPS_DIR}/penv.sh"

echo "========================================"
printf "POWER_DB=%s\n" $POWER_DB
printf "export POWER_DB\n\n"

printf "PDIR=%s\n" $PDIR
printf "export PDIR\n\n"

printf "PATH=\${PATH}:%s\n" $APPS_DIR
printf "export PATH\n\n"
echo "========================================"

