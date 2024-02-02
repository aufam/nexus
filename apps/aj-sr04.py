import py_nexus
import json
import os
import time
import datetime


class Codec(py_nexus.Codec):
    def __init__(self):
        super().__init__()
    
    def decode(self, buffer):
        return buffer[4:] \
            if len(buffer) >= 10 \
                and buffer[0] == ord('G') and buffer[1] == ord('a') and buffer[2] == ord('p') and buffer[3] == ord('=') \
                and buffer[-1] == 0x0d \
            else []


class AJSR04(py_nexus.Device):
    def __init__(self, serial_port: str):
        super().__init__()
        self.distance = 0
        self.serial = py_nexus.SerialHardware(port=serial_port, speed=py_nexus.B9600, timeout=datetime.timedelta(seconds=1), codec=Codec())
    
    def update(self):
        try:
            self.serial.send(text='1')
            response: str = self.serial.receive().to_string()
            # Find the index 
            equals_index = response.find('=')
            mm_index = response.find('mm')

            # Extract the substring between '=' and 'm'
            value_str = response[equals_index + 1:mm_index]

            # Convert the extracted substring to a float
            self.distance = float(value_str) / 1000
        except Exception as e:
            self.distance = 0
        finally:
            print(f'Distance: {self.distance} m')
    
    def path(self) -> str:
        return '/aj-sr04'

    def json(self) -> str:
        data1 = json.loads(self.serial.json)
        data2 = json.loads('{"distance": ' + str(self.distance) + '}')
        return json.dumps({**data1, **data2})
    
    def post(self, method_name: str, json_request: str) -> str:
        return self.serial.post(method_name, json_request)
    
    def patch(self, json_request: str) -> str:
        return self.serial.patch(json_request)
    

def main(host, port, page, serial_port):
    ajsr04 = AJSR04(serial_port=serial_port)
    listener = py_nexus.Listener()
    listener.add(ajsr04)
    listener.interval = datetime.timedelta(seconds=1)

    server = py_nexus.HttpServer()
    server.add(ajsr04)
    
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
        print(f'{request.remote_addr} {request.method} {request.path} {request.version} {response.status} {response.body}')
    
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
    parser = argparse.ArgumentParser(description='AJ-SR04M Interface')

    parser.add_argument('-H', '--host', type=str, default='localhost', help='Specify the server host')
    parser.add_argument('-p', '--port', type=int, default=5000, help='Specify the server port')
    parser.add_argument('-P', '--page', type=str, default=script_directory + '/aj-sr04.html', help='Specify the HTML page')
    parser.add_argument('-s', '--serial-port', type=str, default='auto', help='Specify the serial port')

    args = parser.parse_args()
    main(**vars(args))

