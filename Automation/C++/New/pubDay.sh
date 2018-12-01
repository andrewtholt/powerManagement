#!/bin/bash

export PATH="/home/andrewh/bin:/home/andrewh/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/snap/bin"

mosquitto_pub -t "/test/day" -m $(date "+%j") -h 192.168.0.65 -r




