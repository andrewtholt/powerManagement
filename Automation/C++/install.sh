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
which mysqld
if [ $? -ne 0 ]; then
    echo "Fatal error, no mysqld found"
    exit 1
fi
echo "... done"

