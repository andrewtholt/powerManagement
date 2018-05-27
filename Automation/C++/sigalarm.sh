#!/bin/bash 

set -x

PID=$(ps -ef | grep logic | grep -v grep | awk '{ print $2 }')

kill -SIGALRM $PID

