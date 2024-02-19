import py_nexus
import time


def main():
    try:
        host, port = "127.0.0.1", 5001

        print('[TEST] Create Modbus TCP Server')
        server = py_nexus.ModbusTCPServer()
        print('[OK]')

        coils = [True, False, True]
        inputs = [0x1234, 0x5678, 0xabcd]

        @server.CoilGetter(0x1100)
        def _():
            return coils[0]

        @server.CoilGetter(0x1101)
        def _():
            return coils[1]
        
        @server.CoilGetter(0x1102)
        def _():
            return coils[2]
        
        @server.CoilSetter(0x1100)
        def _(value):
            coils[0] = value

        @server.CoilSetter(0x1101)
        def _(value):
            coils[1] = value
        
        @server.CoilSetter(0x1102)
        def _(value):
            coils[2] = value
        
        @server.AnalogInputGetter(0x2200)
        def _():
            return inputs[0]
        
        @server.AnalogInputGetter(0x2201)
        def _():
            return inputs[1]
        
        @server.AnalogInputGetter(0x2202)
        def _():
            return inputs[2]
        
        @server.logger
        def _(ip, request, response):
            print(f'IP:{ip}, Req:{request.to_vector()}, Res:{response.to_vector()}')
        
        server.listen(host=host, port=port)
        print(f"Server is running on http://{host}:{port}/")
        time.sleep(0.1)

        print('[TEST] Create Modbus TCP Client')
        client = py_nexus.ModbusTCPClient(host=host, port=port)
        print('[OK]')

        print('[TEST] Read Coils')
        res = client.ReadCoils(0x1100, 3)
        if client.error() == py_nexus.ModbusError.NONE:
            assert res == coils
        else:
            print("Modbus error:", client.error())
        print('[OK]')

        print('[TEST] Write Coils')
        client.WriteMultipleCoils(0x1100, 3, [True, True, True])
        if client.error() == py_nexus.ModbusError.NONE:
            assert coils == [True, True, True]
        else:
            print("Modbus error:", client.error())
        print('[OK]')

        print('[TEST] Write Single Coils')
        value = client.WriteSingleCoil(0x1101, False)
        if client.error() == py_nexus.ModbusError.NONE:
            assert value == False
            assert coils == [True, False, True]
        else:
            print("Modbus error:", client.error())
        print('[OK]')

        print('[TEST] Read Input Registers')
        res = client.ReadInputRegisters(0x2200, 3)
        if client.error() == py_nexus.ModbusError.NONE:
            assert res == inputs
        else:
            print("Modbus error:", client.error())
        print('[OK]')

    except AssertionError as e:
        print(f'\033[91m--Assertion Error: {e}--\033[0m')
    
    else:
        print("\033[92m--Assertion Success--\033[0m")

    finally:
        server.stop()
        

if __name__ == '__main__':
    main()
