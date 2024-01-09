#ifndef PROJECT_NEXUS_MODBUS_RTU_C_WRAPPER_H
#define PROJECT_NEXUS_MODBUS_RTU_C_WRAPPER_H

#include <cstdint>
#include <cstddef>
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* nexus_modbus_rtu_server_t;

typedef void* nexus_modbus_rtu_client_t;

nexus_modbus_rtu_server_t nexus_modbus_rtu_server_new(uint8_t server_address, const char* port, speed_t speed);
void nexus_modbus_rtu_server_delete(nexus_modbus_rtu_server_t server);

void nexus_modbus_rtu_server_coil_getter(nexus_modbus_rtu_server_t server, uint16_t register_address, int (*getter)());
void nexus_modbus_rtu_server_coil_setter(nexus_modbus_rtu_server_t server, uint16_t register_address, void (*setter)(int));

void nexus_modbus_rtu_server_holding_register_getter(nexus_modbus_rtu_server_t server, uint16_t register_address, uint16_t (*getter)());
void nexus_modbus_rtu_server_holding_register_setter(nexus_modbus_rtu_server_t server, uint16_t register_address, void (*setter)(uint16_t));

void nexus_modbus_rtu_server_discrete_input_getter(nexus_modbus_rtu_server_t server, uint16_t register_address, int (*getter)());
void nexus_modbus_rtu_server_analog_input_getter(nexus_modbus_rtu_server_t server, uint16_t register_address, uint16_t (*getter)());

void nexus_modbus_rtu_server_exception_status_getter(nexus_modbus_rtu_server_t server, uint8_t (*getter)());
void nexus_modbus_rtu_server_diagnostic_getter(nexus_modbus_rtu_server_t server, uint8_t sub_function, uint8_t* (*getter)(size_t* length));

nexus_modbus_rtu_client_t nexus_modbus_rtu_client_new(const char* port, speed_t speed);
void nexus_modbus_rtu_client_delete(nexus_modbus_rtu_client_t client);

int* nexus_modbus_rtu_client_read_coils(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, size_t* length);
int* nexus_modbus_rtu_client_read_discrete_inputs(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, size_t* length);
uint16_t* nexus_modbus_rtu_client_read_holding_registers(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, size_t* length);
uint16_t* nexus_modbus_rtu_client_read_input_registers(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, size_t* length);

void nexus_modbus_rtu_client_write_single_coil(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, int value);
void nexus_modbus_rtu_client_write_single_register(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t value);

uint8_t nexus_modbus_rtu_client_read_exception_status(nexus_modbus_rtu_client_t client, uint8_t server_address);
uint8_t* nexus_modbus_rtu_client_diagnostic(nexus_modbus_rtu_client_t client, uint8_t server_address, uint8_t sub_function, size_t* length);

void nexus_modbus_rtu_client_write_multiple_coils(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, int* values, size_t length);
void nexus_modbus_rtu_client_write_multiple_registers(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, uint16_t* values, size_t length);

#ifdef __cplusplus
}
#endif
#endif // PROJECT_NEXUS_MODBUS_RTU_C_WRAPPER_H
