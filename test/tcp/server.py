import py_nexus
import time


def main():
    host, port = "127.0.0.1", 5000

    server = py_nexus.TCPServer()

    @server.callback
    def _(request):
        return request

    @server.logger
    def _(ip, request, response):
        print(f'IP:{ip}, Req:{request.to_string()}, Res:{response.to_string()}')
    
    server.listen(host=host, port=port)
    print(f"Server is running on http://{host}:{port}")

    time.sleep(1)

    client = py_nexus.TCPClient(host=host, port=port)
    res = client.request("test")

    assert res.to_string() == "test"

    server.stop()
    print("Server stop")
    print("Assertion success")


if __name__ == '__main__':
    main()
