import py_nexus
import json
import os
import time
import datetime
from math import nan


class URM15(py_nexus.Device):
    def __init__(self, address, serial_port, serial_speed, verbose):
        super().__init__()
        self.rtu = py_nexus.ModbusRTUClient(server_address=address, port=serial_port, speed=serial_speed)
        self.distance = nan
        self.temperature = nan
        self.verbose = verbose
    
    def update(self):
        self.rtu.WriteSingleRegister(0x0008, 0b1101) # send trigger
        
        time.sleep(0.065)

        res = self.rtu.ReadHoldingRegisters(0x0005, 2)
        if self.rtu.error() == py_nexus.ModbusError.NONE:
            self.distance = res[0] * 0.1 if res[0] != 0xFFFF else nan
            self.temperature = res[1] * 0.1 if res[1] != 0xFFFF else nan
        else:
            if self.verbose:
                print(self.rtu.error())
            self.distance = nan
            self.temperature = nan
        
        if self.verbose:
            print(f'Distance: {self.distance:.1f} cm, Temperature: {self.temperature:.1f} C')

    def path(self) -> str:
        return '/urm15'

    def json(self) -> str:
        data1 = json.loads(self.rtu.json())
        data2 = {
            'distance': float(format(self.distance, f'.1f')),
            'temperature': float(format(self.temperature, f'.1f')),
        }
        return json.dumps({**data1, **data2})
    
    def post(self, method_name: str, json_request: str) -> str:
        return self.rtu.post(method_name, json_request)


def setup(address_set, address_prev=0x00, serial_port='auto', speed=19200):
    rtu = py_nexus.ModbusRTUClient(server_address=address_prev, port=serial_port, speed=py_nexus.BaudRate(speed))
    if not rtu.isConnected():
        print('No device connected')
        return

    rtu.WriteSingleRegister(0x0008, 0b0101) # set control register
    print('Set control register')

    rtu.WriteSingleRegister(0x0003, 0x0003) # set baud rate to be 9600
    print('Set baud rate to 9600')

    rtu.WriteSingleRegister(0x0002, address_set) # set address
    print(f'Set address to {address_set}')

    print('Please restart the device')


def main(serial_port, baud, device_address, host, port, page, path_file, do_setup, verbose):
    if baud is None:
        baud = 19200 if do_setup else 9600

    if do_setup:
        setup(device_address, 0x00, serial_port, baud)
        return

    path_file.append(f'/:{page}')

    device = URM15(address=device_address, serial_port=serial_port, serial_speed=py_nexus.BaudRate(baud), verbose=verbose)
    listener = py_nexus.Listener()
    listener.add(device)
    listener.interval = datetime.timedelta(seconds=1)

    server = py_nexus.HttpServer()
    server.add(device)

    for pair in path_file:
        path, file = pair.split(':')

        @server.Get(path)
        def _(request, response):
            try:
                with open(file, 'r') as f:
                    file_content = f.read()
                response.set_content(file_content, py_nexus.getContentType(file))
                response.status = 200
            except Exception as e:
                response.set_content(str(e), 'text/plain')
                response.status = 500

            return response
        
    @server.logger
    def _(request, response):
        print(f'{request.remote_addr} {request.method} {request.path} {request.version} {response.status}')
    
    server.listen(host=host, port=port)
    print(f"Server is running on http://{host}:{port}/")

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nCtrl+C pressed. Exiting...")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
    
    server.stop()
    listener.stop()


if __name__ == '__main__':
    import argparse

    script_directory = os.path.dirname(os.path.abspath(__file__))
    parser = argparse.ArgumentParser(description='URM15 Interface')

    parser.add_argument('-s', '--serial-port', type=str, default='auto', help='Specify the serial port. Default = auto')
    parser.add_argument('-b', '--baud', type=int, default=None, help='Specify the serial baud rate. Default = 9600 or 19200 (setup mode)')
    parser.add_argument('-d', '--device-address', type=int, default=0x0F, help='Specify the device address. Default = 15')
    parser.add_argument('-H', '--host', type=str, default='localhost', help='Specify the server host. Default = localhost')
    parser.add_argument('-p', '--port', type=int, default=5000, help='Specify the server port. Default = 5000')
    parser.add_argument('-P', '--page', type=str, default=script_directory + '/urm15.html', help='Specify the HTML page')
    parser.add_argument('-f', '--path-file', type=str, default=[], action='append', help='Specify the additional path-file pair. eg: /src.js:static/src.js')
    parser.add_argument('-i', '--do-setup', type=int, default=0, help='Setup the device. Default = 0')
    parser.add_argument('-v', '--verbose', type=int, default=0, help='Verbosity level. Default = 0')

    args = parser.parse_args()
    main(**vars(args))

