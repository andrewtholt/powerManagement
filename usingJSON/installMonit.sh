#!/bin/bash

# set -x

SCRIPTS="dispatch.sh  Server.sh syncMQTT.sh"
MONIT="/etc/monit"

if [!  -d "$MONIT" ]; then
    echo "No monit folder in  $MONIT"
    exit 1
fi

sudo mkdir -p ${MONIT}/Scripts
sudo mkdir -p ${MONIT}/conf.d

HERE=$(pwd)
cd Scripts

for S in ${SCRIPTS}; do
    echo "Copying ${S}"
    sudo cp $S ${MONIT}/Scripts
done

cd $HERE/Monit

sudo cp *.monitrc ${MONIT}/conf.d

sudo monit -t  
if [ $? -eq 0 ] ; then
    echo "Config OK."

    sudo monit reload
    sleep 1
    sudo monit summary
fi

