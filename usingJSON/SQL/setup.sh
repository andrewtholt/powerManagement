#!/bin/bash
set -x
mysql -u automation -pautomation automation < ./mysqlSetup.sql
mysql -u automation -pautomation automation < ./tstData.sql

