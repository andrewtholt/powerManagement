#!/usr/bin/env python3

import logicVM as logic

def main():
    machine = logic.logicVM()
    machine.verbose()
    machine.dump()

    if machine.load():
        print("Load failed")
        sys.exit(3)

    machine.run()

main()

