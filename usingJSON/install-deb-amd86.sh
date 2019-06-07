#!/bin/bash

set -x

LEN=`getconf LONG_BIT`

if [ $LEN -ne 64 ]; then
    echo "Attempting to build on wrong platform."
    exit -1
fi

PRODUCTION="NO"
DEBUG="NO"

while getopts ":hdp" opt; do
  case ${opt} in
    h ) echo "Usage: cmd [-h] [-t]"
      ;;
    d ) DEBUG="YES"
        PRODUCTION="NO"
      ;;
    p ) PRODUCTION="YES"
        DEBUG="NO"
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

DEST="${BASE}/opt/homeControl/bin"
LIBS="${BASE}/opt/homeControl/lib"
ETC="${BASE}/opt/homeControl/etc"
SCRIPTS="${BASE}/opt/homeControl/Scripts"
DATA="${BASE}/opt/homeControl/data"
MONIT="${BASE}/etc/monit"


if [ "$DEBUG" == "YES" ]; then
#    ./buildDebug.sh
    echo "./buildDebug.sh"
    PLACE="Debug/src"
fi

if [ "$PRODUCTION" == "YES" ]; then
    ./buildRelease.sh
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

SCRIPTS="dispatch.sh  Server.sh  setup.sh  syncMQTT.sh"

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
dpkg -b ./debian-x86_64 .
echo "Done"
