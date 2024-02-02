import os
import subprocess

print(os.path.dirname(os.path.abspath(__file__)))

def run_test(file_name):
    print(f'--Assertion start: {file_name}--')
    subprocess.run(['python3', os.path.join(os.path.dirname(os.path.abspath(__file__)), file_name)])

if __name__ == '__main__':
    run_test('tools/byte_view.py')
    print()
    run_test('http/server.py')
    print()
    run_test('modbus/tcp.py')
    print()
    run_test('serial/serial.py')
    print()
    run_test('tcp/server.py')
