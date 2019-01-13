#!/bin/bash

set -x

curl -X GET \
    -H "Content-Type: application/json" \
    http://192.168.0.65:8123/api/states/sun.sun
