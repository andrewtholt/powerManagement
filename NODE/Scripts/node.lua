#!/usr/bin/lua
-- 
-- Need to 'pull' this somehow.
--
require 'inifile'

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

function installPkg(name,queue)

    if queue then
        cmd = ("tsp -n sudo apt-get -y install " .. name)
    else
        cmd = ("sudo apt-get -y install " .. name)
    end
    print( cmd )
    rc = os.execute( cmd )
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

-- config = inifile.parse( base .. "Data/whatAmI.ini")
config = inifile.parse( cfgName )

if (config['config']['roles']) == nil then
    roles = {}
else
    print("Roles    " ..  config['config']['roles'])

    roles = (config['config']['roles']):split(",")
end

server = config["network"]['server']

print(server)

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
--        print("=======================")
--        print(pkg)

        file=y["test"]
        act=y["action"]

--        print(file)
--        print(act)

        if file == "NONE" then
            installPkg(pkg,true)
        else
            if fileExists(file) then
                print("File ",file," exists, Package ",pkg," Installed.")
            else
                print("Installing Package ",pkg)
                installPkg(pkg,true)
            end
        end

        if act ~= "NONE" then
            cmd = "tsb -n sudo " .. act
            print(cmd)
            local rc = os.execute( cmd )
        end
        print("=======================")

    end

end

-- body,c,l,h = http.request('http://w3.impa.br/~diego/software/luasocket/http.html')
-- print('status line',l)
-- print('body',body)
-- 
