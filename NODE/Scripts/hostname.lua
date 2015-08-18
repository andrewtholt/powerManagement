#!/usr/bin/lua

function hostname()
    io.input("/etc/hostname")
    t = io.read("*all")
    io.close()
    hst = string.sub(t,1,-2)
    return hst
end
print( hostname() )
