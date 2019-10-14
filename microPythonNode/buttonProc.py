#!/usr/local/bin/micropython

data=0xff

mask = [1,2,4,8,16,32,64,128]

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


