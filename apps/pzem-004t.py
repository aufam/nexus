import py_nexus
import json
import os
import time
import datetime
from math import nan


class PZEM(py_nexus.Device):
    def __init__(self, serial_port: str, address=0xF8):
        super().__init__()
        self.rtu = py_nexus.ModbusRTUClient(port=serial_port, speed=py_nexus.B9600)
        self.address = address
        self.voltage = 0.0
        self.current = 0.0
        self.power = 0.0
        self.energy = 0.0
        self.frequency = 0.0
        self.powerFactor = 0.0
        self.alarm = False
        self.alarmThreshold = 0.0
    
    def update(self):
        res, err = self.rtu.ReadInputRegisters(self.address, 0x0000, 10)
        if err == py_nexus.ModbusError.NONE:
            self.voltage = res[0] * .1
            self.current = (res[1] | res[2] << 16) * .001
            self.power = (res[3] | res[4] << 16) * .1
            self.energy = (res[5] | res[6] << 16) * 1.0
            self.frequency = res[7] * .1
            self.powerFactor = res[8] * .01
            self.alarm = res[9] == 0xFFFF
        else:
            self.voltage = nan
            self.current = nan
            self.power = nan
            self.energy = nan
            self.frequency = nan
            self.powerFactor = nan
            self.alarm = False

        res, err = self.rtu.ReadHoldingRegisters(self.address, 0x0001, 1)
        if err == py_nexus.ModbusError.NONE:
            self.alarmThreshold = res[0]
        else:
            self.alarmThreshold = nan
    
    def path(self) -> str:
        return '/pzem-004t'

    def json(self) -> str:
        data1 = json.loads(self.rtu.json)
        data2 = {
            'address': self.address,
            'voltage': self.voltage,
            'current': self.current,
            'power': self.power,
            'energy': self.energy,
            'frequency': self.frequency,
            'powerFactor': self.powerFactor,
            'alarm': self.alarm,
            'alarmThreshold': self.alarmThreshold
        }
        return json.dumps({**data1, **data2})
    
    def post(self, method_name: str, json_request: str) -> str:
        if (method_name == 'reset_energy'):
            self.rtu.Request([self.address, 0x42])
            return json.dumps({
                'status': 'success',
                'message': 'request to reset energy'
            })
        
        if (method_name == 'calibrate'):
            self.rtu.Request([self.address, 0x41, 0x37, 0x21])
            return json.dumps({
                'status': 'success',
                'message': 'request to calibrate'
            })
            
        return self.rtu.post(method_name, json_request)
    
    def patch(self, json_request: str) -> str:
        request = json.loads(json_request)
        response = {}

        if 'address' in request:
            new_address = request['address']
            res, err = self.rtu.WriteSingleRegister(self.address, 0x0001, new_address)
            if err == py_nexus.ModbusError.NONE:
                self.address = new_address
            response['address'] = new_address

        if 'alarmThreshold' in request:
            alarmThreshold = request['alarmThreshold']
            res, err = self.rtu.WriteSingleRegister(self.address, 0x0002, alarmThreshold)
            if err == py_nexus.ModbusError.NONE:
                self.alarmThreshold = alarmThreshold
            response['alarmThreshold'] = alarmThreshold

        response_succes = {
            'status': 'success',
            'message': 'success update parameter'
        }
        response_fail = {
            'status': 'fail',
            'message': 'Unknown key'
        }
        return json.dumps({**response_succes, **response}) if response else json.dumps(response_fail)
    

def main(host, port, page, serial_port, device_address):
    pzem = PZEM(serial_port=serial_port, address=device_address)
    listener = py_nexus.Listener()
    listener.add(pzem)
    listener.interval = datetime.timedelta(seconds=1)

    server = py_nexus.HttpServer()
    server.add(pzem)
    
    @server.Get('/')
    def _(request, response):
        try:
            with open(page, 'r') as file:
                file_content = file.read()
            response.set_content(file_content, 'text/html')
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
    parser = argparse.ArgumentParser(description='PZEM-004T Interface')

    parser.add_argument('-H', '--host', type=str, default='localhost', help='Specify the server host')
    parser.add_argument('-p', '--port', type=int, default=5000, help='Specify the server port')
    parser.add_argument('-P', '--page', type=str, default=script_directory + '/pzem-004t.html', help='Specify the HTML page')
    parser.add_argument('-s', '--serial-port', type=str, default='auto', help='Specify the serial port')
    parser.add_argument('-d', '--device-address', type=int, default=0xF8, help='Specify the device address')

    args = parser.parse_args()
    main(**vars(args))

