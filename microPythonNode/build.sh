#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: ./build.sh <ap name>"
    exit 1
fi

OUT="app.py"

cat head.py > $OUT

./toAp.py $1 >> $OUT

cat  body.py >> $OUT

cat  tail.py >> $OUT

echo "Output in $OUT"

