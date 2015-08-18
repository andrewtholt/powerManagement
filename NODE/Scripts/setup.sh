#!/bin/sh
set -x

ARCH=$(uname -m)

JAVA="NO"
SERVER="NO"
TOOLS="NO"
DEV="NO"
DESKTOP="NO"
FLAGS="-y -s" # Answer prompt 'Yes' and simulate.

if [ $# -eq 0 ]; then
    echo "Check for config file"
    CFG="/usr/local/etc/config/setup"

    if [ -f "$CFG" ]; then
        echo "Reading config"
        . $CFG
    else
        echo "No config, no options, goodbye."
        exit 1
    fi
fi


exit 0
#
# Add command line option processing here
#
while getopts ":jstpdvh" opt ; do
  case $opt in
    j) JAVA="YES" ;;
    v) VERBOSE="YES" ;;
    s) SERVER="YES" ;;
    t) TOOLS="YES" ;;
    p) DEV="YES" ;;
    d) DESKTOP="YES" ;;
    h)
      printf "Usage: setup.sh -j -s -t -p -d -v -h\n"
      printf "\t-j\tJava\n"
      printf "\t-s\tServer\n"
      printf "\t-t\tTools\n"
      printf "\t-p\tProgramming Tools\n"
      printf "\t-d\tDesktop\n"
      printf "\t-v\tVerbose\n"
      
      ;;
    \?)
      echo "Invalid option"
      exit -1
      ;;
  esac
done

exit 0

apt-get update
apt-get $FLAGS install aptitude

aptitude update

aptitude $FLAGS install apt-file
apt-file update

aptitude $FLAGS install sudo
aptitude $FLAGS install ssh
aptitude $FLAGS install unzip
aptitude $FLAGS install bc

if [ $DEV = "YES" ]; then
  aptitude $FLAGS install build-essential
  aptitude $FLAGS install gdb
  aptitude $FLAGS install bison flex
  aptitude $FLAGS install tcl8.5
  aptitude $FLAGS install libsqlite3-dev sqlite3
  aptitude $FLAGS install libavahi-client-dev
  aptitude $FLAGS install cscope
  aptitude $FLAGS install git
  aptitude $FLAGS install subversion
  aptitude $FLAGS install libtool
  aptitude $FLAGS install gawk
  aptitude $FLAGS install sqlite
  aptitude $FLAGS install cmake
  aptitude $FLAGS install libncursesw5-dev
  aptitude $FLAGS install strace
  aptitude $FLAGS install ltrace
  
#  aptitude $FLAGS install doxygen doxygen-gui
fi
#
# The multilibs are for Intel and AMD only
#
if [ $ARCH = "x86_64" -o $ARCH = "amd64" ]; then
  echo "64 bit x86"
  aptitude $FLAGS install gcc-multilib
  aptitude $FLAGS install g++-multilib
fi
#
#
if [ $JAVA = "YES" ]; then
  aptitude $FLAGS install openjdk-7-jdk
fi
aptitude $FLAGS install htop
aptitude $FLAGS install iotop
aptitude $FLAGS install dos2unix
aptitude $FLAGS install vim

aptitude $FLAGS install gettext
aptitude $FLAGS install screen
#
# Server
#
if [ $SERVER = "YES" ]; then
  aptitude $FLAGS install lxc
fi
#
# Optional, but useful network tools
#
if [ $TOOLS = "YES" ]; then
  aptitude $FLAGS install tcpdump
  aptitude $FLAGS install nmap
  aptitude $FLAGS install traceroute
  aptitude $FLAGS install fping
  aptitude $FLAGS install dnsutils
  aptitude $FLAGS install snmp
fi
# aptitude $FLAGS install exuberant-ctags
#
# Optional
# Desktop
#
if [ $DESKTOP = "YES" ]; then
  aptitude $FLAGS install terminator
# aptitude $FLAGS install facter
# aptitude $FLAGS install dia
  if [ $DEV = "YES" ]; then
    aptitude $FLAGS install kate
    aptitude $FLAGS install meld
  fi
fi
#
# Dev
#
#
# aptitude $FLAGS install libboost1.53-dev # C++ STL, required to build libyaml-cpp
#
# Ubuntu x86 only
#
# add-apt-repository ppa:danielrichter2007/grub-customizer
# apt-get update
# aptitude update
# aptitude $FLAGS install grub-customizer
#
# End Ubuntu only
#
# Build & install latest yED, monit, libCello, spread, cross compilers from codesourcery.c
