#ifndef PROJECT_NEXUS_MODBUS_API_CLIENT_H
#define PROJECT_NEXUS_MODBUS_API_CLIENT_H

#include "nexus/modbus/api.h"

#ifdef __cplusplus

namespace Project::nexus::modbus::api {
    class Client : virtual public abstract::Client {
    public:
        Client(int server_address) : server_address(server_address) {}
        virtual ~Client() {}

        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;

        std::vector<bool> ReadCoils(uint16_t register_address, uint16_t n_register);
        std::vector<bool> ReadDiscreteInputs(uint16_t register_address, uint16_t n_register);
        
        std::vector<uint16_t> ReadHoldingRegisters(uint16_t register_address, uint16_t n_register);
        std::vector<uint16_t> ReadInputRegisters(uint16_t register_address, uint16_t n_register);
        
        bool WriteSingleCoil(uint16_t register_address, bool value);
        uint16_t WriteSingleRegister(uint16_t register_address, uint16_t value);
        
        uint8_t ReadExceptionStatus();
        std::vector<uint8_t> Diagnostic(uint8_t sub_function);

        void WriteMultipleCoils(uint16_t register_address, uint16_t n_register, const std::vector<bool>& values);
        void WriteMultipleRegisters(uint16_t register_address, uint16_t n_register, const std::vector<uint16_t>& values);
    
        Error error() const { return error_; }

    protected:
        int server_address;
        Error error_ = Error::NONE;
    };
}

#else
typedef void* nexus_modbus_client_t;

void nexus_modbus_client_delete(nexus_modbus_client_t client);

int* nexus_modbus_client_read_coil(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register);
int* nexus_modbus_client_read_discrete_inputs(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register);

uint16_t* nexus_modbus_client_read_holding_registers(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register);
uint16_t* nexus_modbus_client_read_input_registers(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register);

int nexus_modbus_client_write_single_coil(nexus_modbus_client_t client, uint16_t register_address, int value);
uint16_t nexus_modbus_client_write_single_register(nexus_modbus_client_t client, uint16_t register_address, uint16_t value);

uint8_t nexus_modbus_client_read_exception_status(nexus_modbus_client_t client);
uint8_t* nexus_modbus_client_diagnostic(nexus_modbus_client_t client, uint8_t sub_function, size_t* length);

void nexus_modbus_client_write_multiple_coils(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register, const int* values, size_t length);
void nexus_modbus_client_write_multiple_registers(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register, const uint16_t* values, size_t length);

#endif
#endif // PROJECT_NEXUS_MODBUS_API_CLIENT_H