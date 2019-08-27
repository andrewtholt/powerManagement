#!/bin/bash

# set -x

PROCS="Server tDispatch mqtt"

for n in $PROCS; do
    ps -ef | grep $n | grep -v grep
done
