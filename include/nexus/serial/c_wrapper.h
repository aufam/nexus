#ifndef PROJECT_NEXUS_SERIAL_C_WRAPPER_H
#define PROJECT_NEXUS_SERIAL_C_WRAPPER_H

#include "nexus/abstract/c_wrapper.h"
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Nexus serial object handler
typedef void* nexus_serial_t;

/// Creates a new Nexus serial object.
/// @param port Name of the serial port to connect to.
/// @param speed Baud rate for serial communication.
/// @return A handle to the created Nexus serial object, or NULL on error.
nexus_serial_t nexus_serial_new(const char* port, speed_t speed);

/// Deletes a Nexus serial object.
/// @param serial Handle to the Nexus serial object to delete.
void nexus_serial_delete(nexus_serial_t serial);

/// Reconnects a Nexus serial object to a different port or baud rate.
/// @param serial Handle to the Nexus serial object to reconnect.
/// @param port New port name to connect to.
/// @param speed New baud rate to use for communication.
void nexus_serial_reconnect(nexus_serial_t serial, const char* port, speed_t speed);

/// Disconnects the serial connection for a Nexus serial object.
/// @param serial Handle to the Nexus serial object to disconnect.
void nexus_serial_disconnect(nexus_serial_t serial);

/// Checks if a Nexus serial object is currently connected.
/// @param serial Handle to the Nexus serial object to check.
/// @return 1 if connected, 0 if not connected.
int nexus_serial_is_connected(nexus_serial_t serial);

/// Sends a text message through the serial connection.
/// @param serial Handle to the Nexus serial object to use for sending.
/// @param text Text message to send.
/// @return Number of bytes sent, or -1 on error.
int nexus_serial_send_text(nexus_serial_t serial, const char *text);

/// Receives a string message from the serial connection.
/// @param serial Handle to the Nexus serial object to use for receiving.
/// @param timeout Timeout in milliseconds.
/// @return Pointer to the received string (user must deallocate using free()),
///         or NULL on timeout or error.
char* nexus_serial_receive_text(nexus_serial_t serial, int timeout);

/// Sends bytes through the serial connection.
/// @param serial Handle to the Nexus serial object to use for sending.
/// @param data data to send.
/// @param length[out] data length.
/// @return Number of bytes sent, or -1 on error.
int nexus_serial_send_bytes(nexus_serial_t serial, uint8_t *data, size_t length);

/// Receives bytes of data from the serial connection.
/// @param serial Handle to the Nexus serial object to use for receiving.
/// @param timeout Timeout in milliseconds.
/// @param length[out] data length.
/// @return Pointer to the received data (user must deallocate using free()),
///         or NULL on timeout or error.
uint8_t* nexus_serial_receive_bytes(nexus_serial_t serial, int timeout, size_t* length);

/// Adds a callback function to be invoked when data is received.
/// @param serial Handle to the Nexus serial object to add the callback to.
/// @param fn Callback function to be called when data is received.
///           It receives the received data and its length as arguments.
void nexus_serial_add_callback(nexus_serial_t serial, void (*fn)(const uint8_t* data, size_t len));

/// Function pointer type to override encode and decode method.
/// @param member Pointer to additional member of Serial override.
/// @param data Input data to encode / decode.
/// @param len[in, out] Pointer to input data length and output data length
/// @return Encoded / decoded output data.
typedef uint8_t* (*nexus_serial_codec_function_t)(void* member, const uint8_t* data, size_t* len);

/// Creates a new Nexus serial object with overriden functions.
/// @param encoder Encoder function pointer.
/// @param decoder Decoder function pointer.
/// @param port Name of the serial port to connect to.
/// @param speed Baud rate for serial communication.
/// @return A handle to the created Nexus serial object, or NULL on error.
nexus_serial_t nexus_serial_override_new(nexus_restful_t restful, 
    nexus_serial_codec_function_t encoder,
    nexus_serial_codec_function_t decoder,
    const char* port, speed_t speed
);

/// Deletes a Nexus serial object that was created by nexus_serial_override_new.
/// @param serial Handle to the Nexus serial object to delete.
void nexus_serial_override_delete(nexus_serial_t serial);

#ifdef __cplusplus
}
#endif
#endif // PROJECT_NEXUS_SERIAL_C_WRAPPER_H
