import subprocess
from time import sleep

def main():
    running="true"
    bufsize=1024

    cmd = "toSpread -l  /tmp/py.log"
#    cmd = "./tst.sh"
    
    p = subprocess.Popen(cmd, shell=True, bufsize=bufsize, stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=False)

#    sleep(60)
    (child_stdin, child_stdout) = (p.stdin, p.stdout)

    line = child_stdout.readline()
    print line

    child_stdin.write("^set PASSTHROUGH true\n")
    child_stdin.flush()

    child_stdin.write("^connect\n")
    child_stdin.flush()

    line = child_stdout.readline()

    print "Here"
    print line
    print ".. to"
#    exec(line)
    running = "true"

    while running == "true":
        print "Running ..."
        line = child_stdout.readline()
        print ">" + line + "<"
#        exec(line)

    print "Bye"


main()
