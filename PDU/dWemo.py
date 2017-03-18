import os 
import paho.mqtt.client as mqtt

class wemo:

    state = 'UNKNOWN'
    name =''
    port=49152
    conn = None
    verbose=False

    def __init__(self,name):
        self.name=name

    def setVerbose(self,v):
        self.verbose=v

    def getStatus(self):

        cmd="/usr/local/bin/wemo_control.sh %s GETSTATE" % ( self.name )

        if self.verbose:
            print(cmd)

        res=os.popen( cmd )

        state=(res.readline().strip())

        return(state)

    def setStatus(self, state):

        s = state.upper()

        cmd="/usr/local/bin/wemo_control.sh %s %s" % ( self.name, s )

        if self.verbose:
            print(cmd)
        res=os.popen( cmd )

        state=(res.readline().strip())

        return(state)



if __name__ == "__main__":

    device='PorchLight'
    porchLight = wemo(device)

    s=porchLight.getStatus()

    print(device + ' is ' + s )

    s=porchLight.setStatus('OFF')
    print(s)

    s=porchLight.setStatus('ON')
    print(s)


