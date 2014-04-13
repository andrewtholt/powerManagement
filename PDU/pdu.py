#
# A general PDU class.
#
#
from os import system,popen
from sys import exit

class apcPdu:
    cfgFile = "/tmp/power.cfg"

    nameList = [ "OUT1", "OUT2","OUT3","OUT4","OUT5","OUT6","OUT7","OUT8" ]
    portState = [ "UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN" ]
    stateList = [ "ERROR","ON","OFF","REBOOT","UNKNOWN","ON_DELAY","OFF_DELAY","REBOOT_DELAY"]

    ip_address = "127.0.0.1"
    port_count = 8
    oid_base = ".1.3.6.1.4.1.318.1.1.4.4.2.1.3."

    snmpSetOn    = "snmpset -m ALL -v1 -c private apc sPDUOutletCtl.%d i 1"
    snmpSetOff   = "snmpset -m ALL -v1 -c private apc sPDUOutletCtl.%d i 2"
    snmpStatus   = "snmpget -OQven -m ALL -v1 -c public apc sPDUOutletCtl.%d"
    snmpState    = "snmpget -OQven  -v1 -c public -m ALL apc sPDUMasterState.0"
    apcPortNames = "snmpwalk -OQven -v1 -c public  -m ALL apc sPDUOutletCtlName"
    


    def __init__(self,ip):
        self.ip_address = ip


    def dump(self):
        print "IP Address :", self.ip_address

        for idx in range(0,self.port_count):
            print "%02d : %-20s %-10s" % (idx,self.nameList[idx],self.portState[idx])

    def powerOn(self,name):
        port = -1
        try:
            idx =  self.nameList.index(name)
        except ValueError:
            return

        port = idx + 1
        system( self.snmpSetOn % port + " > /dev/null" )
        self.portState[idx] = "ON"
        
    def powerOff(self,name):
        port = -1
        try:
            port =  self.nameList.index(name)
        except ValueError:
            return

        port = pidx + 1
        system( self.snmpSetOff % port + " > /dev/null")
        self.portState[idx] = "OFF"

    def allOff(self):
        allOff="snmpset -m ALL -v1 -c private apc sPDUMasterControlSwitch.0 i 3 > /dev/null"
        system( allOff )
        self.getAll()

    def allOn(self):
        allOn="snmpset -m ALL -v1 -c private apc sPDUMasterControlSwitch.0 i 1 > /dev/null"
        system( allOn )
        self.getAll()


    def rename(self,old,new):
        try:
            idx =  self.nameList.index(old)
            self.nameList[idx] = new
        except ValueError:
            pass

    def powerStatus(self,name):
        state = 0
        idx =  self.nameList.index(name)
        return( self.portState[idx])
        
        try:
            idx =  self.nameList.index(name)
            data = popen( self.snmpStatus % (idx + 1)).readline()
            state =  data.strip() 
            tmp = self.stateList[ int(state)]
            self.portState[idx]= tmp
        except ValueError:
            pass
        return state

    def getAll(self):
        self.nameList = []
        data = popen( self.apcPortNames).readlines()
        
        for p in data:
            self.nameList.append( (p.strip()).strip('"') )

        data = popen( self.snmpState).readline()
        data = data.upper()

        self.portState =  (data[1:-3]).split()
        
        

if __name__ == "__main__":
    tst = apcPdu("apc")
    tst.getAll()
    tst.dump()
    tst.rename("OUT8","fan")
    tst.powerOn("Fans")

    print tst.powerStatus("Out1")
    print tst.powerStatus("Fans")


    tst.allOn()
    tst.dump()
    tst.powerOff("fan")
    tst.allOff()
    tst.dump()
