#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: ./build.sh <ap name>"
    exit 1
fi

cat head.py > app.py

./toAp.py $1 >> app.py

cat  body.py >> app.py

cat  tail.py >> app.py
