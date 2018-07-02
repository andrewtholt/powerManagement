#!/bin/bash

# set -x

HERE=$(pwd)

echo "First check for /etc/service entry ..."
SVC=$(grep myclient /etc/services | wc -l)

if [ $SVC -eq 0 ]; then
    echo "Fatal error, myclient not in services."
    echo "# " >> /etc/services
    echo "# ATH custom service here" >> /etc/services
    echo "# " >> /etc/services
    echo "myclient    10001/tcp           # Custom MySql client" >> /etc/services
    echo "# " >> /etc/services
fi

echo "... done"

echo "Check for mysql ..."
SERVER=$(dpkg -l | grep mysql-server | wc -l)

if [ $SERVER -eq 0 ]; then
    echo "Error, no mysqld found."
    echo "Installing ... "
fi
echo "... done"

 mysql -u root -p < ../mySqlSetup.sql
 cd ..
./loader.py -v -i data.txt -t MYSQL -d automation -v

