import json

class device:
    def __init__(self):
        self.base=""
        self.loc=""
        self.power="NODATA"
        self.sensor = None
        self.state = None
        self.dirty = False  # Set to tru if something changes.

        self.verbose=True
        self.type = 'UNKNOWN'

    def setPower(self,p):
        t = p.upper()

        if t != self.power:
            self.dirty=True

        self.power = t

    def getPower(self):

        if self.power in ('ON','TRUE'):
            return True;
        else:
            return False

    def setBase(self,b):
        self.base = b

    def setLocation(self,l):
        self.loc=l

    def getState(self):
        pass

    def setState(self, state):
        pass

    def setSensor(self,s):
        pass

    def getDirty(self):
        t = self.dirty
        self.dirty=False
        return t

    def dumpState(self):
        print("Type     >" + self.type + "<")
        print("Base     >" + self.base + "<")
        print("Location >" + self.loc + "<")
        print("Power    >" + self.power + "<")
        print("Sensor   >" + str(self.sensor) + "<")
        print("State    >" + str(self.state) + "<")
        print("Dirty    >" + str(self.dirty) + "<")


class sonata(device):

    def __init__(self):
        device.__init__(self)
        self.type="SONATA"

    def getState(self):
        # 
        # Not empty
        #
        if self.state:
            return self.state['POWER']
        else:
            return 'UNKNOWN'

    def setState(self, s):
        if self.state==None:
            print("HERE")
            self.dirty=True
            self.state = json.loads(s)
            self.setPower(self.state['POWER'])
        else:
            print("NOT HERE")
            if self.state == s:
                self.dirty=False
            else:
                self.dirty=True
                self.state = s

    def setSensor(self,s):
        self.sensor=s

if __name__ == "__main__":
    tst = sonata()

    tst.setState('{"Time":"2018-02-25T15:42:00", "Uptime":389, "Vcc":3.212, "POWER":"OFF", "Wifi":{"AP":1, "SSID":"HoltAtHome4", "RSSI":38, "APMac":"1C:AF:F7:38:C7:E0"}}')

    tst.dumpState()

    print("getState >" + tst.getState() + "<")




