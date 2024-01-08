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
        return "/a"
    
    def update(self):
        self.x += 1


class B(Device):
    def __init__(self):
        super().__init__()

    def path(self):
        return "/b"
    
    def update(self):
        self.x -= 1


def main():
    host, port = "localhost", 5000

    a, b = A(), B()
    assert isinstance(a, py_nexus.Restful)
    assert isinstance(a, py_nexus.Device)
    assert isinstance(b, py_nexus.Restful)
    assert isinstance(b, py_nexus.Device)

    server = py_nexus.HttpServer().add(a).add(b)
    assert isinstance(server, py_nexus.HttpServer)

    @server.Get("/test")
    def _(request, response):
        response.status = 200
        response.set_content('{"msg": "echo test"}', "application/json")
        return response

    @server.logger
    def _(request, response):
        print(f'{request.remote_addr} {request.method} {request.path} {request.version} {response.status} {response.body}')
    
    server.listen(host=host, port=port)
    print(f"Server is running on http://{host}:{port}")

    time.sleep(1)
    a.update()
    b.update()

    client = py_nexus.HttpClient(host=host, port=port)
    ra = client.Get("/a")
    rb = client.Get("/b")
    rt = client.Get("/test")

    ja = json.loads(ra.body)
    jb = json.loads(rb.body)
    jt = json.loads(rt.body)

    assert ja['x'] == 1
    assert jb['x'] == -1
    assert jt['msg'] == 'echo test'

    server.stop()
    print("Server stop")

    print("Start listener")
    listener = py_nexus.Listener().add(a).add(b)

    time.sleep(1)

    assert a.x > 1
    assert b.x < -1

    listener.stop()
    print("Stop listener")
    print("Assertion success")


import asyncio

def request(path):
    client = py_nexus.HttpClient(host="localhost", port=5000)
    return client.Get(path)

async def request_async(path):
    loop = asyncio.get_event_loop()
    result = await loop.run_in_executor(None, request, path)
    return result

async def main_async():
    server = py_nexus.HttpServer()

    @server.Get("/test")
    def _(request, response):
        response.status = 200
        response.set_content('{"msg": "echo test"}', "application/json")
        return response

    @server.logger
    def _(request, response):
        print(f'{request.remote_addr} {request.method} {request.path} {request.version} {response.status} {response.body}')

    server.listen(host="localhost", port=5000)
    
    response = await request_async("/test")
    print(response.body)

    server.stop()


if __name__ == '__main__':
    main()
    # asyncio.run(main_async())
