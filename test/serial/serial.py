import py_nexus
import time
from datetime import timedelta

def main():
    ser = py_nexus.Serial(port='auto', speed=py_nexus.B115200)
    if not ser.isConnected:
        print("No hardware serial is connected")
        return

    @ser.callback
    def _(data):
        print('From callback 1:', ''.join(chr(num) for num in data))

    @ser.callback
    def _(data):
        print('From callback 2:', ''.join(chr(num) for num in data))
    
    @ser.path_override
    def _():
        return '/custom_serial_path'

    @ser.decode_override
    def _(data):
        # only allow if data starts with 'e' and ends with '\n'
        return data if len(data) >= 2 and chr(data[0]) == 'e' and chr(data[-1]) == '\n' else []

    ser.send(text='echo\n')
    echo = ser.receiveText(filter=lambda x: x == 'echo\n', timeout=timedelta(milliseconds=100))
    print('From blocking:', echo)
    time.sleep(1)

    ser.send(text='echo 1\n')
    time.sleep(1)
    
    ser.send(text='echo 2\n')
    time.sleep(1)
    
    ser.send(text='_echo 3\n') # print echo 3
    time.sleep(1)

    print(ser.path)


if __name__ == '__main__':
    main()