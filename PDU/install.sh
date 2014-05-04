#!/bin/sh

DESTDIR="/opt/power"
printf "Destination [%s]:" $DESTDIR
read TMP

if [ ! -z $TMP ]; then
    DESTDIR=tmp
fi

PDIR="${DESTDIR}"
export PDIR

POWER_DB="${DESTDIR}/data/power.db"
export POWER_DB

echo "Add the following to your environment."

echo "========================================"
printf "POWER_DB=%s\n" $POWER_DB
printf "export POWER_DB\n\n"

printf "PDIR=%s\n" $PDIR
printf "export PDIR\n\n"

printf "PATH=\${PATH}:%s/app\n" $PDIR
printf "export PATH\n\n"
echo "========================================"

