#!/bin/bash
# set -x
VERSION=$(cat ./version.txt)

HW=$(uname -m)

if [ $HW = "x86_64" ]; then
    HW=amd64
fi

if [ $HW = "armv7l" ]; then
    HW=armhf
fi


echo "Package: home-control"
echo "Section: misc"
echo "Priority: extra"
echo "Maintainer: Andrew Holt"
echo "Architecture: ${HW}"
echo "Version: ${VERSION}"
echo "Depends: monit"
echo "Description: Home Automation.."
