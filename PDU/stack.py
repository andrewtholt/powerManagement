#!/usr/bin/env python3

class stackClass:
    def __init__(self):
        self.theStack = []

    def push(self, data):
        self.theStack.append( data )

    def pop(self):
        return( self.theStack.pop() )

    def swap(self):
        l = len(self.theStack) - 1

        x=self.theStack[l]
        self.theStack[l]=self.theStack[l-1]
        self.theStack[l-1]=x

    def dup(self):
        l = len(self.theStack) - 1
        x=self.theStack[l]
        self.theStack.append( x )

    def drop(self):
        self.theStack.pop()

    def depth(self):
        return len(self.theStack)

    def dump(self):
        for n in self.theStack:
            print(str(n) + " ", end="")
        print()

    def And(self):
        a=self.theStack.pop()
        b=self.theStack.pop()
        self.theStack.append( a and b )

    def Or(self):
        a=self.theStack.pop()
        b=self.theStack.pop()
        self.theStack.append( a or b )

if __name__ == "__main__":
    st = stackClass()

    st.push("one")
    st.push("two")
    st.dump()

    st.swap()
    st.dump()

    st.dup()
    st.dump()

    st.push(True)
    st.push(False)
    st.And()
    st.dump()

    print(st.depth())

    print(st.pop())

