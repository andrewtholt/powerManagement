#!/bin/bash

# set -x

if [ $# -eq 0 ]; then
    echo "Select a config"
    LIST=$(ls /etc/mqtt/*.json)

    for N in $LIST; do
        if [ ! -L $N ]; then
            printf "\t"
            basename $N | cut -f1 -d"." | cut -f2 -d"_"
        else
            printf "Current config ->"
            ls -l /etc/mqtt/bridge.json  | cut -f2 -d"_" | cut -f1 -d "."
        fi
    done
    exit 1
else
    cd /etc/mqtt
    sudo rm -f bridge.json
    sudo ln -s bridge_$1.json ./bridge.json
fi

