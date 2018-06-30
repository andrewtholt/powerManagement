#!/bin/bash

set -x

mosquitto_pub -h 192.168.0.65 -t /test/StartSwitch -m ON -r
mosquitto_pub -h 192.168.0.65 -t /test/StartSwitch -m OFF -r

