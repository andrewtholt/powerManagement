#!/bin/bash

set -x

echo "First check for /etc/service entry ..."
SVC=$(grep myclient /etc/services | wc -l)

if [ $SVC -eq 0 ]; then
    echo "Fatal error, myclient not in services."
    exit 1
fi

echo "... done"

echo "Check for mysql ..."
SERVER=$(dpkg -l | grep mysql-server | wc -l)

if [ $SERVER -eq 0 ]; then
    echo "Fatal error, no mysqld found."
    echo "Please install."
    exit 1
fi
echo "... done"

