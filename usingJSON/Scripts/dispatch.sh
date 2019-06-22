#!/bin/sh

# set -x

PATH="/opt/homeControl/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

export PATH

NAME="dispatch"
CONFIG=""
ARGS=""
HOSTNAME=$(hostname)
PIDFILE="/var/run/$NAME.pid"
TOPIC="/home/office/$HOSTNAME/$NAME"

MQTT="127.0.0.1"
if [ -x "/usr/bin/jq" ]; then
    MQTT=$(cat /etc/mqtt/bridge.json | jq -r .local.name )
fi

getPid() {
    PID=$(ps -ef | grep $1 | grep -v grep | awk '{ print $2 }')
    return $PID
}

pubStatus() {
    if [ ! -z "$TOPIC" ]; then
        mosquitto_pub -h $MQTT -t $TOPIC -m $1 -r
    fi
}

status() {
    RET=1

    if [ -f "$PIDFILE" ]; then
        PID=$(cat $PIDFILE)

        kill -0 $PID > /dev/null 2>&1
        if [ $? -eq 0 ] ; then
            RET=0
        else
            RET=1
        fi
    else
        RET=1
    fi

    if [ $RET -eq 1 ]; then
        pubStatus "DOWN"
    else
        pubStatus "UP"
    fi

    return $RET
}

if [  $# -ne 1 ]; then
    echo "Usage: $NAME status|start|stop"
    exit 0
fi

case "$1" in
    status)
        status
        ;;
    start)
        echo "Start"
        status
        if [  $RET -eq 1 ]; then
            echo "START"
            setup.sh
            $NAME $ARGS
            sleep 1
        else
            echo "RUNNING"
        fi
        status
        ;;
    stop)
        echo "Stop"
        status
        if [  $RET -eq 1 ]; then
            echo "Not running"
        else
            kill $(cat $PIDFILE)
        fi

        if [ -f $PIDFILE ]; then
            kill $(cat $PIDFILE)
            rm -f $PIDFILE
            sleep 1
        else
            echo "No PID file."
        fi
        status
        ;;
esac

exit $?


