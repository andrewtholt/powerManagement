#!/bin/bash 

set -x

mysql -h 192.168.10.65 -u automation -p automation < ./mysqlNewSetup.sql
