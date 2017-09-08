#!/bin/bash

set -x

echo $#

if [ $# -ne 2 ]; then
    echo "Usage: $0 <topic> <message>"
    exit 1
fi

/usr/local/bin/mosquitto_pub -t "$1" -m "$2"
exit 0

