import py_nexus
import sys

def hex_args_to_int_list(hex_args):
    int_list = []
    for hex_value in hex_args:
        try:
            int_value = int(hex_value, 16)
            int_list.append(int_value)
        except ValueError:
            print(f"Error: Unable to convert '{hex_value}' to an integer.")
    return int_list

def main():
    # Skip the first argument (script name) and get the remaining arguments
    hex_args = sys.argv[1:]
    if not hex_args:
        print("Error: No hex values provided.")
        return
    
    int_list = hex_args_to_int_list(hex_args)
    encoded = py_nexus.ModbusEncode(int_list)
    print(' '.join([f'{value:02x}' for value in encoded]))

if __name__ == "__main__":
    main()