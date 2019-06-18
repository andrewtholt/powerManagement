#!/bin/bash

set -x

TITLE=""
BODY=""

while getopts ":hb:t:" opt; do
    case ${opt} in
        b) BODY=$OPTARG
            ;;
        t) TOPIC=$OPTARG
            ;;
        h) echo "Usage:"
            ;;
        *) echo "Usgae:"
        esac
done

if [ -z "$TOPIC" ]; then
    echo "Needs a topic"
    exit 1
fi

if [ -z "$BODY" ]; then
    echo "Needs a body"
    exit 1
fi

echo "Body :$BODY"
echo "Topic:$TOPIC"


TOKEN=$(cat token.txt)

curl --silent -u "${TOKEN}:" -d type="note" -d body="${BODY}" -d title="${TOPIC}" 'https://api.pushbullet.com/v2/pushes'

echo ""
