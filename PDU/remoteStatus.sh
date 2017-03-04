#!/bin/sh

if [ $# -eq 0 ]; then
    echo "Usage $0 <ip|name>"
    exit 1
fi

echo $1

ssh $1 ". /opt/power/apps/penv.sh; power status all"
