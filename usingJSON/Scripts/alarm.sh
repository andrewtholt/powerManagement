#!/bin/bash 

PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
export PATH

HOSTS="punch acs1 beaglexm raspberrypi acs4" 

# set -x
MQTT=$(cat /etc/mqtt/bridge.json | jq -r .local.name )

for N in $HOSTS; do
    STATE="DOWN"
    fping -q $N

    if [ $? -eq 0 ]; then 
        STATE="UP"
#    elif [ $? -eq 1 ] ; then
#        STATE="DOWN"
    fi
    echo "$N $STATE"
    TOPIC="/home/office/$N/state"
    echo $TOPIC
    mosquitto_pub -r -t  "/home/office/$N/state" -m "$STATE" -h $MQTT
done



