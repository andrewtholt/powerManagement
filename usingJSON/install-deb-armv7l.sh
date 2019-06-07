#!/bin/bash

# set -x

LEN=$(getconf LONG_BIT)

if [ $LEN -ne 32 ]; then
    echo "Attempting to build on wrong platform."
    exit -1
fi

PRODUCTION="NO"
DEBUG="NO"
TEST="NO"

while getopts ":hdpt" opt; do
  case ${opt} in
    h ) echo "Usage: cmd [-h] [-t]"
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

BASE="./debian-armv7l"
./mkControl.sh > ${BASE}/DEBIAN/control


DEST="${BASE}/opt/homeControl/bin"
LIBS="${BASE}/opt/homeControl/lib"
ETC="${BASE}/opt/homeControl/etc"
SCRIPTS="${BASE}/opt/homeControl/Scripts"
DATA="${BASE}/opt/homeControl/data"
MONIT="${BASE}/etc/monit"

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

BINS="dispatch pubSunRiset  Server  syncMQTT"

for B in $BINS; do
    cp $PLACE/${B} $DEST
done

cp Scripts/setup.sh  $DEST

SCRIPTS="dispatch.sh  Server.sh syncMQTT.sh"

if [ ! -d $MONIT ]; then
    mkdir -p $MONIT
    mkdir -p $MONIT/Scripts
    mkdir -p $MONIT/conf.d
fi

for S in $SCRIPTS; do
    cp Scripts/$S  $MONIT/Scripts
done

MONIT_CFG="dispatch.monitrc  Server.monitrc  syncMQTT.monitrc"
pwd
for C in $MONIT_CFG; do
    cp Monit/$C  $MONIT/conf.d
done

echo "Make package"
# if [ "$TEST" == "NO" ]; then
    dpkg -b ./debian-armv7l .
# fi
echo "Done"
