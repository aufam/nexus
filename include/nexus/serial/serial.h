#ifndef PROJECT_NEXUS_SERIAL_SERIAL_H
#define PROJECT_NEXUS_SERIAL_SERIAL_H

#ifdef __cplusplus
#include "nexus/abstract/communication.h"
#include "nexus/tools/byte_view.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <vector>
#include <termios.h>

namespace Project::nexus::serial {

    /// Concrete implementation of a communication interface for hardware serial communication.
    class Serial : virtual public abstract::Communication {
    public:
        struct Default {
            static constexpr const char* port = "auto"; 
            static constexpr speed_t speed = B115200;
            static constexpr std::chrono::milliseconds timeout = std::chrono::milliseconds(100); 
        };

        /// Arguments for constructing a `Serial` object.
        struct Args { 
            std::string port=Default::port; 
            speed_t speed=Default::speed;
            std::chrono::milliseconds timeout=Default::timeout; 
        };

        /// Constructor with specified arguments.
        explicit Serial(Args args);
        Serial(std::string port, speed_t speed, std::chrono::milliseconds timeout=Default::timeout) 
            : Serial({.port=port, .speed=speed, .timeout=timeout}) {}

        /// Default constructor using default configuration.
        Serial() : Serial({.port=Default::port, .speed=Default::speed, .timeout=Default::timeout}) {}

        /// Destructor for proper cleanup.
        virtual ~Serial();
        
        /// Returns the path for accessing the serial resource.
        std::string path() const override { return "/serial"; }

        /// Returns a JSON representation of the serial status.
        std::string json() const override;

        /// Handles POST requests to invoke serial operations.
        std::string post(std::string_view method_name, std::string_view json_string) override;

        /// Reconnects the serial communication with current settings.
        virtual void reconnect();

        /// Reconnects the serial communication with new settings.
        virtual void reconnect(Args args);

        /// Disconnects the serial communication.
        virtual void disconnect();

        /// Checks if the serial communication is connected.
        /// @return True if connected, false otherwise.
        virtual bool isConnected() const;

        /// Encode the raw bytes.
        /// @return Encoded message.
        virtual std::vector<uint8_t> encode(nexus::byte_view buffer) const;

        /// Decode the raw bytes.
        /// @return Decoded message.
        virtual std::vector<uint8_t> decode(nexus::byte_view buffer) const;

        /// Sends text over the serial connection.
        /// @param text Text to send.
        /// @return Number of bytes sent, or -1 if failed.
        virtual int send(std::string_view text);

        /// Sends buffer over the serial connection.
        /// @param buffer Buffer to send.
        /// @return Number of bytes sent, or -1 if failed.
        virtual int send(nexus::byte_view buffer);

        /// Receives text over the serial connection.
        /// Blocks until text is received or the specified timeout occurs.
        /// @param timeout Timeout in milliseconds. Defaults to 1000ms.
        /// @return The received text, or an empty string if no text was received within the timeout.
        virtual std::string receiveText(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));

        /// Receives text over the serial connection, applying a filter function.
        /// Blocks until text matching the filter is received or the specified timeout occurs.
        /// @param filter A function that takes a string and returns true if it matches the desired criteria.
        /// @param timeout Timeout in milliseconds. Defaults to 1000ms.
        /// @return The received text that matches the filter, or an empty string if no matching text was received within the timeout.
        virtual std::string receiveText(std::function<bool(std::string_view)> filter, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));

        /// Receives raw bytes over the serial connection.
        /// Blocks until bytes are received or the specified timeout occurs.
        /// @param timeout Timeout in milliseconds. Defaults to 1000ms.
        /// @return A vector of received bytes.
        virtual std::vector<uint8_t> receiveBytes(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));

        /// Receives raw bytes over the serial connection, applying a filter function.
        /// Blocks until bytes matching the filter are received or the specified timeout occurs.
        /// @param filter A function that takes a vector of bytes and returns true if it matches the desired criteria.
        /// @param timeout Timeout in milliseconds. Defaults to 1000ms.
        /// @return A vector of received bytes that match the filter.
        virtual std::vector<uint8_t> receiveBytes(std::function<bool(nexus::byte_view)> filter, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));

        /// Adds a callback function to be invoked when data is received.
        /// @param fn The callback function to be called, taking a vector of received bytes as its argument.
        virtual void addCallback(std::function<void(nexus::byte_view)> fn);

    protected:
        Args args_;

        // file descriptor
        int fd = -1;

        // tx and rx buffer
        uint8_t rxBuffer[256] = {};
        std::mutex txMutex;
        std::mutex rxMutex;

        // received message
        std::vector<uint8_t> receivedMessage;
        std::mutex receivedMessageMutex;
        std::condition_variable receivedMessageCondition;

        // worker thread
        std::vector<std::function<void(nexus::byte_view)>> callbacks;

    private:
        std::atomic<bool> isRunning = true;
        std::thread worker;
        void work();
    };
}

#else
#include "nexus/serial/c_wrapper.h"
#endif
#endif
