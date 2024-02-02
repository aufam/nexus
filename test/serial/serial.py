import py_nexus
import time
import threading
import queue
import json

class Serial(py_nexus.Serial):
    def __init__(self):
        super().__init__()
        self.shared_queue = queue.Queue()
        self.is_running = False
        self.producer_thread = threading.Thread(target=self._producer)
        self.reconnect()
    
    def reconnect(self):
        self.is_running = True
        self.producer_thread.start()
    
    def disconnect(self):
        self.is_running = False
        self.producer_thread.join()
        self.shared_queue.put(None)
    
    def isConnected(self) -> bool:
        return self.is_running
    
    def send(self, buffer: py_nexus.ByteView) -> int:
        self.shared_queue.put(py_nexus.ByteView(buffer))
        return len(buffer)
    
    def receive(self, receiveFilter=None) -> py_nexus.ByteView:
        if receiveFilter == None:
            return self.shared_queue.get()

        for retry in range(5):
            data = self.shared_queue.get()
            if receiveFilter(data):
                return data
        
        return py_nexus.ByteView([])
    
    def _producer(self):
        i = 0
        while self.is_running:
            time.sleep(0.1)
            self.shared_queue.put(py_nexus.ByteView(f'Produce {i}'))
            i += 1


def main():
    ser = Serial()

    try:
        print('[TEST] Send and receive data')
        ser.send('From Send')
        res = ser.receive().to_string()
        assert res == 'From Send'
        print('[OK]')

        print('[TEST] Receive data from producer')
        res = ser.receive().to_string()
        assert res == 'Produce 0'
        print('[OK]')

        print('[TEST] Receive data from producer with filter')
        res = ser.receive(lambda x: x[-1] == ord('3')).to_string()
        assert res == 'Produce 3'
        print('[OK]')

        print('[TEST] Send and receive data using Restful')
        ser.post('send', '{"buffer": [1, 2, 3]}')
        res = json.loads(ser.post('receive', ''))['res']
        assert res == [1, 2, 3]
        print('[OK]')

    except AssertionError as e:
        print(f'--AssertionError: {e}--')
    
    else:
        print("--Assertion success--")
    
    finally:
        ser.disconnect()


if __name__ == '__main__':
    main()