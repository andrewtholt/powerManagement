#!/bin/sh

# set -x
PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
export PATH
MQTT=$(cat /etc/mqtt/bridge.json | jq -r .local.name )

DATA=$(apcaccess status | egrep "^LINEV|^BCHARGE|^TIMELEFT|^LOADPCT" | awk '{ printf"%s:%s\n",$1, $3 }')

for D in $DATA; do
    T=$(echo $D | cut -f1 -d":")
    M=$(echo $D | cut -f2 -d":")

    mosquitto_pub -h $MQTT -t "/home/office/ups/$T" -m $M -r
done


