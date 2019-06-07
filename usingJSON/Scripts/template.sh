#!/bin/sh

# set -x

PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
export PATH

NAME="fred"
CONFIG=""
ARGS="-x -y -z"
HOSTNAME=$(hostname)
PIDFILE="/var/run/$NAME.pid"
TOPIC="/home/office/$HOSTNAME/$NAME"

getPid() {
    PID=$(ps -ef | grep $1 | grep -v grep | awk '{ print $2 }')
    return $PID
}
pubStatus() {
    if [ ! -z "$TOPIC" ]; then
        mosquitto_pub -r -h picobob -t $TOPIC -m $1
    fi
}

status() {
    RET=1

    if [ -f "$PIDFILE" ]; then
        pubStatus "UP"
        RET=0
    else
        pubStatus "DOWN"
        RET=1
    fi

    return $RET
}

if [  $# -ne 1 ]; then
    echo "Usage: $NAME status|start|stop"
    exit 0
fi


case "$1" in
    status)
#        status
        ;;
    start)
        echo "Start"
        # 
        # First check for pid file exists.
        #
        if [ -f "$PIDFILE" ]; then
            # It does, is it valid ?
            #
            TPID=$(cat $PIDFILE)
            kill -0 $TPID
            if [ $? -eq 0 ]; then
                # Something with that pid exists, so we fail
                #
                echo "Already running with pid $TPID"
            fi
        else
            $NAME $ARGS
        fi

        #        nohup $NAME > /dev/null 2>&1  &
        #        sleep 1
        #        getPid $NAME
        #        echo $? > $PIDFILE
        ;;
    stop)
        echo "Stop"

        if [ -f $PIDFILE ]; then
            kill $(cat $PIDFILE)
            rm -f $PIDFILE
            sleep 1
        else
            echo "No PID file."
        fi
        ;;
esac

status
exit $?


