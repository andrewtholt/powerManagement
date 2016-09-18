#!/bin/sh

if [ $# -ne 3 ]; then
    echo "Usage $0 <ip|name> <port name> <on|off|status>"
    exit 1
fi


# set -x

HOST=$1
PORT=$2
STATE=$3

if [ $STATE = "status" ]; then
    ssh $1 ". /opt/power/apps/penv.sh; power $STATE $PORT "
fi

if [ $STATE = "on" -o $STATE = "off" ]; then
    ssh $1 ". /opt/power/apps/penv.sh; power $STATE $PORT; fastAct "
fi
