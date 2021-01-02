#!/bin/bash

# set -x

LEN=$(getconf LONG_BIT)

if [ $LEN -ne 64 ]; then
    echo "Attempting to build on wrong platform."
    exit -1
fi

PRODUCTION="NO"
DEBUG="NO"
TEST="NO"

while getopts ":hdpt" opt; do
  case ${opt} in
    h ) echo "Usage: cmd [-h] [-t] -p|-d"
      ;;
    d ) DEBUG="YES"
        PRODUCTION="NO"
      ;;
    p ) PRODUCTION="YES"
        DEBUG="NO"
      ;;
    t ) TEST="YES"
      ;;
    \? ) echo "Usage: cmd [-h] [-t]"
      ;;
  esac
done

if [  $PRODUCTION = "NO" ] && [ $DEBUG = "NO"  ]; then
    echo "Invalid, choose either production or debug."
    exit 1
fi


echo "Build debian package"
echo "First make everything."

HERE=$(pwd)

BASE="./debian-x86_64"
./mkControl.sh > ${BASE}/DEBIAN/control


DEST="${BASE}/opt/homeControl/bin"
LIBS="${BASE}/opt/homeControl/lib"
ETC="${BASE}/opt/homeControl/etc"
SCRIPTS="${BASE}/opt/homeControl/Scripts"
CLIENTS="${BASE}/opt/homeControl/Clients"
DATA="${BASE}/opt/homeControl/data"
MONIT="${BASE}/etc/monit"

rm -rf $BASE/etc
rm -rf $BASE/opt

mkdir -p $DEST

if [ "$DEBUG" == "YES" ]; then
    echo "./buildDebug.sh"
    if [ "$TEST" == "NO" ]; then
        ./buildDebug.sh
    fi
    PLACE="Debug/src"
fi

if [ "$PRODUCTION" == "YES" ]; then
    echo "./buildRelease.sh"

    if [ "$TEST" == "NO" ]; then
        ./buildRelease.sh
    fi

    PLACE="Release/src"
fi

SO="libaseClasses.so"

mkdir -p $LIBS

for S in $SO; do
    cp $PLACE/../commonClasses/$S $LIBS
done

mkdir -p $SCRIPTS 
SCR="pubTime.sh pubDay.sh alarm.sh powerCron.txt"
for S in $SCR; do
    cp ${HERE}/Scripts/${S} $SCRIPTS
done

PYTHON="pbNotify.py syncMQTT.py mqtt.py snmp.py getPower.py setPower.py"
PYTH_DIR="Python"
for B in $PYTHON; do
    cp $PYTH_DIR/${B} $DEST
done

CFG=${PLACE}/../../JSON
if [ ! -d $CFG ]; then
    echo "No folder $PLACE"
    exit 1
fi

mkdir -p ${BASE}/etc/mqtt
cp ${CFG}/*.json ${BASE}/etc/mqtt

BINS="porchLights backLights tDispatch pubSunRiset Server ../Server2/Server2"

for B in $BINS; do
    cp $PLACE/${B} $DEST
done

cp Scripts/setup.sh  $DEST

SCRIPT_LIST="dispatch.sh Server.sh syncMQTT.sh mqtt.sh snmp.sh backLights.sh porchLight.sh"

if [ ! -d $MONIT ]; then
    mkdir -p $MONIT
    mkdir -p $MONIT/Scripts
    mkdir -p $MONIT/conf.d
    mkdir -p $MONIT/conf-enabled
fi

for S in $SCRIPT_LIST; do
    cp Scripts/$S  $MONIT/Scripts
done

mkdir -p $CLIENTS
mkdir -p $CLIENTS/Forth/ficl
mkdir -p $CLIENTS/Forth/atlast

cp $PLACE/../../Clients/Forth/ficl/*.fth $CLIENTS/Forth/ficl
cp $PLACE/../../Clients/Forth/atlast/*.atl $CLIENTS/Forth/atlast

MONIT_CFG="dispatch.monitrc Server.monitrc  syncMQTT.monitrc snmp.monitrc mqtt.monitrc backLights.monitrc porchLight.monitrc"
pwd
for C in $MONIT_CFG; do
    cp Monit/$C  $MONIT/conf.d
done
cp $HERE/Monit/monitrc $MONIT/monitrc.example
# chmod 0600 $MONIT/monitrc
set +x

echo "Make package"
# if [ "$TEST" == "NO" ]; then
    dpkg -b ./debian-x86_64 .
# fi
echo "Done"
