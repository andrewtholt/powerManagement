#!/bin/sh

if [ $# -eq 0 ]; then
    echo "Usage $0 <ip|name>"
fi

echo $1

ssh $1 ". /opt/power/apps/penv.sh; power status all"
