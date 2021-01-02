#!/usr/local/bin/micropython

data=0xff

mask = [0x01,
        0x02,
        0x04,
        0x08,
        0x10,
        0x20,
        0x40,
        0x80]

print(data)


def main():

    for i in range(0,8):
        d = data & mask[i]
        print(i,mask[i],d)

        result = "button" + '{:02d}'.format(i)
        if d == 0:
            print(result, "ON")
        else:
            print(result,"OFF")



main()


