import py_nexus
import json
import os
import time
import datetime
from math import nan


class SHZK(py_nexus.Device):
    def __init__(self, serial_port: str, address=0x01):
        super().__init__()
        self.rtu = py_nexus.ModbusRTUClient(server_address=address, port=serial_port, speed=py_nexus.B9600)
        self.frequencyRunning = 0.0
        self.busVoltage = 0.0
        self.outputVoltage = 0.0
        self.outputCurrent = 0.0
        self.outputPower = 0.0
        self.outputTorque = 0.0
        self.loadSpeed = 0.0
        self.state = -1
        self.faultInfo = -1
    
    def update(self):
        res = self.rtu.ReadHoldingRegisters(0x1001, 6)
        if self.rtu.error() == py_nexus.ModbusError.NONE:
            self.frequencyRunning = res[0]
            self.busVoltage = res[1] * .1
            self.outputVoltage = res[2]
            self.outputCurrent = res[3] * 0.01
            self.outputPower = res[4]
            self.outputTorque = res[5]
        else:
            self.frequencyRunning = nan
            self.busVoltage = nan
            self.outputVoltage = nan
            self.outputCurrent = nan
            self.outputPower = nan
            self.outputTorque = nan

        time.sleep(0.001)

        res = self.rtu.ReadHoldingRegisters(0x100F, 1)
        if self.rtu.error() == py_nexus.ModbusError.NONE:
            self.loadSpeed = res[0]
        else:
            self.loadSpeed = nan

        time.sleep(0.001)

        res = self.rtu.ReadHoldingRegisters(0x3000, 1)
        if self.rtu.error() == py_nexus.ModbusError.NONE:
            self.state = res[0]
        else:
            self.state = -1

        time.sleep(0.001)

        res = self.rtu.ReadHoldingRegisters(0x8000, 1)
        if self.rtu.error() == py_nexus.ModbusError.NONE:
            self.faultInfo = res[0]
        else:
            self.faultInfo = -1
    
    def path(self) -> str:
        return '/shzk'

    def json(self) -> str:
        data1 = json.loads(self.rtu.json())
        data2 = {
            'frequencyRunning': self.frequencyRunning,
            'busVoltage': self.busVoltage,
            'outputVoltage': self.outputVoltage,
            'outputCurrent': self.outputCurrent,
            'outputPower': self.outputPower,
            'outputTorque': self.outputTorque,
            'loadSpeed': self.loadSpeed,
            'state': self.state,
            'faultInfo': self.faultInfo
        }
        return json.dumps({**data1, **data2})
    
    def post(self, method_name: str, json_request: str) -> str:
        if method_name == 'forward_running':
            res = self.rtu.WriteSingleRegister(0x2000, 0x0001)
            if self.rtu.error() == py_nexus.ModbusError.NONE:
                return json.dumps({
                    'status': 'success',
                    'message': 'request to forward running'
                })
        
        if method_name == 'reverse_running':
            res = self.rtu.WriteSingleRegister(0x2000, 0x0002)
            if self.rtu.error() == py_nexus.ModbusError.NONE:
                return json.dumps({
                    'status': 'success',
                    'message': 'request to reverse running'
                })
            
        if method_name == 'forward_jog':
            res = self.rtu.WriteSingleRegister(0x2000, 0x0003)
            if self.rtu.error() == py_nexus.ModbusError.NONE:
                return json.dumps({
                    'status': 'success',
                    'message': 'request to forward jog'
                })
        
        if method_name == 'reverse_jog':
            res = self.rtu.WriteSingleRegister(0x2000, 0x0004)
            if self.rtu.error() == py_nexus.ModbusError.NONE:
                return json.dumps({
                    'status': 'success',
                    'message': 'request to reverse jog'
                })
        
        if method_name == 'free_stop':
            res = self.rtu.WriteSingleRegister(0x2000, 0x0005)
            if self.rtu.error() == py_nexus.ModbusError.NONE:
                return json.dumps({
                    'status': 'success',
                    'message': 'request to free stop'
                })
        
        if method_name == 'decelerate_stop':
            res = self.rtu.WriteSingleRegister(0x2000, 0x0006)
            if self.rtu.error() == py_nexus.ModbusError.NONE:
                return json.dumps({
                    'status': 'success',
                    'message': 'request to decelerate stop'
                })
        
        if method_name == 'fault_resetting':
            res = self.rtu.WriteSingleRegister(0x2000, 0x0007)
            if self.rtu.error() == py_nexus.ModbusError.NONE:
                return json.dumps({
                    'status': 'success',
                    'message': 'request to fault resetting'
                })
            
        return self.rtu.post(method_name, json_request)
    

def main(serial_port, device_address, host, port, page, path_file):
    path_file.append(f'/:{page}')

    device = SHZK(serial_port=serial_port, address=device_address)
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
    parser = argparse.ArgumentParser(description='SHZK Interface')

    parser.add_argument('-s', '--serial-port', type=str, default='auto', help='Specify the serial port')
    parser.add_argument('-d', '--device-address', type=int, default=0x01, help='Specify the device address')
    parser.add_argument('-H', '--host', type=str, default='localhost', help='Specify the server host')
    parser.add_argument('-p', '--port', type=int, default=5000, help='Specify the server port')
    parser.add_argument('-P', '--page', type=str, default=script_directory + '/shzk.html', help='Specify the HTML page')
    parser.add_argument('-f', '--path-file', type=str, default=[], action='append', help='Specify the additional path-file pair. eg: /src.js:static/src.js')

    args = parser.parse_args()
    main(**vars(args))

