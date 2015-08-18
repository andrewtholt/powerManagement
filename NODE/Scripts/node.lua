#!/usr/bin/lua

function hostname()
    io.input("/etc/hostname")
    t = io.read("*all")
    io.close()
    hst = string.sub(t,1,-2)
    return hst
end

function fileExists(name)
    local f=io.open(name,"r")
    if f~=nil then 
        io.close(f) 
        return true 
    else 
        return false 
    end
end


function tspInstalled() 
    if fileExists("/usr/bin/tsp") then
        print("Task Spooler installed.")
    else
        print("Installing Task Spooler ...")
        cmd = "sudo apt-get -y install task-spooler"
        local rc = os.execute( cmd )
    end
end

tspInstalled()
iam = hostname()

if iam == "raspberrypi" then
    print("Not safe to run any further on manager node.")
    os.exit(1)
end

print( iam )

