#!/usr/bin/lua
-- 
--
require 'inifile'

function idSystem()
    cmd = "uname -s -m"

    f = io.popen( cmd )
    data=f:read()

    tmp=data:split(" ")

    return tmp[1], tmp[2]
end

function getInstalledPkgs()
    cmd = "dpkg -l | grep ^ii | awk  '{ print $2 }'"

    f = io.popen( cmd )

    pkgList = {}

    for pkg in f:lines() do
        tmp=pkg:split(":")

        pkgList[tmp[1]] = "INSTALLED"
    end

    return pkgList
end

function string:split( inSplitPattern, outResults )
    if not outResults then
        outResults = { }
    end
    local theStart = 1
    local theSplitStart, theSplitEnd = string.find( self, inSplitPattern, theStart )
    while theSplitStart do
        table.insert( outResults, string.sub( self, theStart, theSplitStart-1 ) )
        theStart = theSplitEnd + 1
        theSplitStart, theSplitEnd = string.find( self, inSplitPattern, theStart )
    end
    table.insert( outResults, string.sub( self, theStart ) )
    return outResults
end

installed=getInstalledPkgs()

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

    if installed["task-spooler"] then 
        print("Task Spooler installed.")
        return
    end

    if fileExists("/usr/bin/tsp") then
        print("Task Spooler installed.")
    else
        print("Installing Task Spooler ...")
        cmd = "sudo apt-get -y install task-spooler"
        local rc = os.execute( cmd )
    end
end

function installPkg(name,queue)

    if installed[name] then
        print(name .. ":Installed.")
        return
    end

    if queue then
        cmd = ("tsp -n sudo apt-get -y install " .. name)
    else
        cmd = ("sudo apt-get -y install " .. name)
    end
    print( cmd )
    rc = os.execute( cmd )
end

opSys,cpu=idSystem()

if opSys ~= "Linux" then
    print("Can't deal with " .. opSys .. " yet.")
    os.exit(3)
end

tspInstalled()
installPkg("lua-socket",false)

http=require'socket.http'
iam = hostname()

if iam == "raspberrypi" then
    print("Not safe to run any further on manager node.")
    os.exit(1)
end

print( iam )
base = "/var/manage/"
cfgName = base .. "Data/" .. iam .. "/whatAmI.ini"

print(cfgName)
if not fileExists( cfgName ) then
    print("Fatal error, I don't know what I am !")
    os.exit(2)
end

config = inifile.parse( cfgName )

if (config['config']['roles']) == nil then
    roles = {}
else
    print("Roles    " ..  config['config']['roles'])

    roles = (config['config']['roles']):split(",")
end

server = config["network"]['server']

print(server)
print("=======================")


table.insert( roles,1,"base" )

for k,v in pairs(roles) do
    path = base .. "Data/" .. v .. ".ini"

    if not fileExists( path ) then
        print("Download " .. v .. " config file")
        url = "http://" .. server .. "/manage/Data/" .. v .. ".ini"

        body,c,l,h = http.request( url )

        io.output( path, 'w')
        io.write( body )
        io.close()
    end
    print("Load " .. v .. " config file")

    tmp = inifile.parse( path )

    for pkg,y in pairs(tmp) do

        file=y["test"]
        act=y["action"]

        installPkg(pkg,true)
    end

    if act ~= "NONE" then
        cmd = "tsp -n sudo " .. act
        print(cmd)
        local rc = os.execute( cmd )
    end
    print("=======================")

end

-- end

-- body,c,l,h = http.request('http://w3.impa.br/~diego/software/luasocket/http.html')
-- print('status line',l)
-- print('body',body)
-- 
