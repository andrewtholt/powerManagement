
class setOnce:
    defined=False
    value=None

    def __init__(self,v=None):
        self.set(v)

    def set(self, v):
        if self.defined:
            pass
        else:
            self.value = v
            self.defined=True

    def get(self):
        return self.value


if __name__ == "__main__":
    test1 = setOnce("Hello")
    print(test1.get())





