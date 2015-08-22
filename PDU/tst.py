#!/usr/bin/python

from powerClass import powerClass as power

control=power()

control.dump()

host='Fans'
state=control.status( host )

print host,state

control.on( host,2 )


