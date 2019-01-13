#!/bin/bash

set -x

curl -v -X GET \
    -H "Content-Type: application/json" \
    http://192.168.0.65:8123/api/states/switch.porch_light
