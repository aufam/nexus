import py_nexus
import time
import asyncio

class Device(py_nexus.Device):
    def __init__(self):
        super().__init__()
        self.x = 0

    def update(self):
        self.x += 1

class Listener(py_nexus.Listener):
    def __init__(self):
        super().__init__()

    # def json(self):
    #     return "Custom listener"

def main():
    j = Listener()
    a = Device()
    j.add(a)
    time.sleep(5)
    print(a.x)
    print(j.json)
    j.stop()

async def async_example():
    num = await py_nexus.async_function(2)
    print(num)

if __name__ == '__main__':
    # asyncio.run(async_example())
    main()
