
#
# . this into a running shell
#

HERE=`pwd`
PDIR="${HERE}"
export PDIR

POWER_DB="${HERE}/data/power.db"
export POWER_DB
#
# Scheduling settings
#
TS_ONFINISH=$PDIR/onfinish.py
export TS_ONFINISH
