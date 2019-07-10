#!/bin/bash

# set -x
# 
# Check if user 'power' exists, if not create.
#
USER="power"
QUEUE="toDispatcher"

getent passwd $USER > /dev/null 2&>1

if [ $? -eq 0  ]; then
    echo "User $USER exists"
else
    echo "User $USER does not exist, create ..."
    sudo useradd --create-home --system power
    # 
    # Append to cron powerCron.txt
    #
    sudo crontab -u power powerCron.txt
    echo "... done."
fi

sudo usermod -s /bin/false power

if [ -f "/dev/mqueue/${QUEUE}" ]; then
    echo "Message Q ${QUEUE} exists."
else
    echo "Message Q ${QUEUE} does not exist, creating ..."
    mqMake -q $QUEUE
    echo "... done."
fi

echo "Fixing ${QUEUE} properties ..."
sudo chown power:power /dev/mqueue/toDispatcher
sudo chmod 660 /dev/mqueue/toDispatcher
echo "... done."

