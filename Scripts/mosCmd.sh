#!/bin/bash

# set -x

if [ $# -ne 2 ]; then
    echo "Usage: $0 <topic> <message>"
    exit 1
fi

/usr/local/bin/mosquitto_pub -r -t "$1" -m "$2"
exit 0

