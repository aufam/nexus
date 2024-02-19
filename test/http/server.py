import py_nexus
import time
import json


class Device(py_nexus.Device):
    def __init__(self):
        super().__init__()
        self.x = 0

    def json(self):
        return '{' + f'"x": {self.x}' + '}'


class A(Device):
    def __init__(self):
        super().__init__()
    
    def path(self):
        return '/a'
    
    def update(self):
        self.x += 1


class B(Device):
    def __init__(self):
        super().__init__()

    def path(self):
        return '/b'
    
    def update(self):
        self.x -= 1


def main():
    host, port = "localhost", 5000

    try:
        print('[TEST] Create Restful and Device objects')
        a, b = A(), B()
        assert isinstance(a, py_nexus.Restful)
        assert isinstance(a, py_nexus.Device)
        assert isinstance(b, py_nexus.Restful)
        assert isinstance(b, py_nexus.Device)
        print('[OK]')

        print('[TEST] Create Server object')
        server = py_nexus.HttpServer().add(a).add(b)
        assert isinstance(server, py_nexus.HttpServer)
        print('[OK]')

        @server.Get('/test')
        def _(request, response):
            response.status = 200
            response.set_content('{"msg": "echo test"}', 'application/json')
            return response

        @server.logger
        def _(request, response):
            print(f'{request.remote_addr} {request.method} {request.path} {request.version} {response.status} {response.body}')
        
        server.listen(host=host, port=port)
        print(f'Server is running on http://{host}:{port}/')

        time.sleep(0.1)
        a.update()
        b.update()

        print('[TEST] Create Client object')
        client = py_nexus.HttpClient(host=host, port=port)
        assert isinstance(client, py_nexus.HttpClient)
        print('[OK]')

        print('[TEST] Client Get')
        ra = client.Get('/a')
        rb = client.Get('/b')
        rt = client.Get('/test')

        ja = json.loads(ra.body)
        jb = json.loads(rb.body)
        jt = json.loads(rt.body)

        assert ja['x'] == 1
        assert jb['x'] == -1
        assert jt['msg'] == 'echo test'
        print('[OK]')

        print('[TEST] Create Listener object')
        listener = py_nexus.Listener().add(a).add(b)
        assert isinstance(listener, py_nexus.Restful)
        print('[OK]')

        time.sleep(0.11)

        print('[TEST] Listener item values')
        assert a.x > 1
        assert b.x < -1
        print('[OK]')

    except AssertionError as e:
        print(f'\033[91m--Assertion Error: {e}--\033[0m')
    
    else:
        print("\033[92m--Assertion Success--\033[0m")

    finally:
        server.stop()
        listener.stop()


if __name__ == '__main__':
    main()
