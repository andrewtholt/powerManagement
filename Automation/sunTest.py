#!/usr/bin/env python3

from Sun import Sun
import time

coords = {'longitude' : -2.6, 'latitude' : 53.6 }
# coords = {'longitude' : 53, 'latitude' : -2.6 }

sun = Sun()

# Sunrise time UTC (decimal, 24 hour format)

now =  time.gmtime()
print("    ", time.strftime("%H:%m:%S",now))

sr = sun.getSunriseTime( coords )['decimal']
print("Sunrise " , sr)
print("    " + sun.toHMS( sr ))

# Sunset time UTC (decimal, 24 hour format)
ss = sun.getSunsetTime( coords )['decimal']
print("Sunset " , ss )
print("    " + sun.toHMS( ss ))
