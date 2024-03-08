import py_nexus
import json
import os
import time
import datetime


class Codec(py_nexus.Codec):
    def __init__(self):
        super().__init__()
    
    def decode(self, buffer):
        return py_nexus.ByteView(
            buffer[4:] \
            if len(buffer) >= 10 \
                and buffer[:4].to_string() == 'Gap=' \
                and buffer[-1] == 0x0d \
            else []
        )


class AJSR04(py_nexus.Device):
    def __init__(self, serial_port: str):
        super().__init__()
        self.distance = 0
        self.serial = py_nexus.SerialHardware(
            port=serial_port, 
            speed=py_nexus.B9600, 
            timeout=datetime.timedelta(seconds=1), 
            codec=Codec()
        )
    
    def update(self):
        self.serial.send(py_nexus.ByteView('1'))
        response: str = self.serial.receive().to_string()
        self.distance = float(response) / 1000 if response else 0
        print(f'Distance: {self.distance} m')

    def path(self) -> str:
        return '/aj-sr04'

    def json(self) -> str:
        data1 = json.loads(self.serial.json())
        data2 = {"distance": float(format(self.distance, f'.3f')) }
        return json.dumps({**data1, **data2})
    
    def post(self, method_name: str, json_request: str) -> str:
        return self.serial.post(method_name, json_request)
    
    def patch(self, json_request: str) -> str:
        return self.serial.patch(json_request)
    

def main(serial_port, host, port, page, path_file):
    path_file.append(f'/:{page}')

    ajsr04 = AJSR04(serial_port=serial_port)
    listener = py_nexus.Listener()
    listener.add(ajsr04)
    listener.interval = datetime.timedelta(seconds=1)

    server = py_nexus.HttpServer()
    server.add(ajsr04)

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
    print(f'Server is running on http://{host}:{port}/')

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print('\nCtrl+C pressed. Exiting...')
    except Exception as e:
        print(f'An unexpected error occurred: {e}')
    
    server.stop()
    listener.stop()


if __name__ == '__main__':
    import argparse

    script_directory = os.path.dirname(os.path.abspath(__file__))
    parser = argparse.ArgumentParser(description='AJ-SR04M Interface')

    parser.add_argument('-s', '--serial-port', type=str, default='auto', help='Specify the serial port')
    parser.add_argument('-H', '--host', type=str, default='localhost', help='Specify the server host')
    parser.add_argument('-p', '--port', type=int, default=5000, help='Specify the server port')
    parser.add_argument('-P', '--page', type=str, default=script_directory + '/aj-sr04.html', help='Specify the HTML page')
    parser.add_argument('-f', '--path-file', type=str, default=[], action='append', help='Specify the additional path-file pair. eg: /src.js:static/src.js')

    args = parser.parse_args()
    main(**vars(args))

