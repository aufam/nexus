import py_nexus
import time


def main():
    try:
        host, port = "127.0.0.1", 5002

        print('[TEST] Create TCP Server')
        server = py_nexus.TCPServer()
        print("[OK]")

        @server.callback
        def _(request):
            return request

        @server.logger
        def _(ip, request, response):
            print(f'IP:{ip}, Req:{request.to_string()}, Res:{response.to_string()}')
        
        server.listen(host=host, port=port)
        print(f"Server is running on http://{host}:{port}/")
        time.sleep(0.1)

        print('[TEST] Create TCP Client')
        client = py_nexus.TCPClient(host=host, port=port)
        assert isinstance(client, py_nexus.Client)
        print("[OK]")

        print('[TEST] TCP Client Request')
        res = client.request(py_nexus.ByteView("test"))
        assert res.to_string() == "test"
        print("[OK]")

    except AssertionError as e:
        print(f'\033[91m--Assertion Error: {e}--\033[0m')
    
    else:
        print("\033[92m--Assertion Success--\033[0m")
    
    finally:
        server.stop()


if __name__ == '__main__':
    main()
