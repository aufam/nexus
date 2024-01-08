import py_nexus
import time


def main():
    host, port = "127.0.0.1", 5000

    server = py_nexus.ModbusTCPServer()
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
    print(f"Server is running on http://{host}:{port}")

    time.sleep(1)

    client = py_nexus.ModbusTCPClient(host=host, port=port)
    res, err = client.ReadCoils(0x1100, 3)
    if err == py_nexus.ModbusError.NONE:
        assert res == coils
    else:
        print("Modbus error:", err)

    err = client.WriteMultipleCoils(0x1100, 3, [True, True, True])
    if err == py_nexus.ModbusError.NONE:
        assert coils == [True, True, True]
    else:
        print("Modbus error:", err)

    value, err = client.WriteSingleCoil(0x1101, False)
    if err == py_nexus.ModbusError.NONE:
        assert value == False
        assert coils == [True, False, True]
    else:
        print("Modbus error:", err)

    res, err = client.ReadInputRegisters(0x2200, 3)
    if err == py_nexus.ModbusError.NONE:
        assert res == inputs
    else:
        print("Modbus error:", err)

    server.stop()
    print("Server stop")
    print("Assertion success")


if __name__ == '__main__':
    main()
