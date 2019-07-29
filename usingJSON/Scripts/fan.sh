#!/bin/bash

# set -x

if [ $# -eq 0 ]; then
    echo "Usage: ./fan.sh on|off"
    exit 1
fi

STATE=2

if [ "$1" == "on" ]; then
    STATE=1
fi


snmpset -m ALL -v1 -c private apc .1.3.6.1.4.1.318.1.1.4.4.2.1.3.8 i $STATE
