#ifndef PROJECT_NEXUS_ABSTRACT_LISTENER_H
#define PROJECT_NEXUS_ABSTRACT_LISTENER_H

#include "nexus/abstract/device.h"

#ifdef __cplusplus
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace Project::nexus::abstract { 

    /// Class that manages a collection of devices and listens for updates,
    /// providing a RESTful interface for interaction.
    class Listener : virtual public Restful {
    public:
        Listener();

        /// Disabled copy constructor to prevent unintended object copies.
        Listener(const Listener&) = delete;

        /// Disabled copy assignment operator to prevent unintended object assignments.
        Listener& operator=(const Listener&) = delete;

        /// Virtual destructor for proper cleanup of resources.
        virtual ~Listener();

        /// Stop thread.
        void stop();

        /// Returns a JSON representation of the listener and its devices.
        std::string json() const override;

        /// Handles POST requests to invoke methods on devices.
        /// @param [in] method_name Name of the method to invoke.
        /// @param [in] json_request JSON-formatted request data.
        /// @return JSON-formatted response from the devices.
        std::string post(std::string_view method_name, std::string_view json_request) override;

        /// Adds a device to the listener.
        /// @param [in] device Unique pointer to the device to add.
        /// @return Reference to the listener for method chaining.
        virtual Listener& add(std::unique_ptr<Device> device);

        /// Removes a device from the listener by index.
        /// @param [in] index Index of the device to remove.
        /// @return Reference to the listener for method chaining.
        virtual Listener& remove(int index);

        /// Accesses a device by index (const version).
        /// @param [in] index Index of the device to access.
        /// @return Reference to the device at the specified index.
        const Device& operator[](int index) const;

        /// Accesses a device by index (non-const version).
        /// @param [in] index Index of the device to access.
        /// @return Reference to the device at the specified index.
        Device& operator[](int index);

        /// Iterator for accessing devices.
        template <typename T>
        class iterator;

        /// Returns a const iterator to the beginning of the devices.
        iterator<const Device> begin() const;

        /// Returns a const iterator to the end of the devices.
        iterator<const Device> end() const;

        /// Returns an iterator to the beginning of the devices.
        iterator<Device> begin();

        /// Returns an iterator to the end of the devices.
        iterator<Device> end();

        /// Returns the number of devices in the listener.
        virtual size_t len() const { return devices.size(); }

        std::chrono::milliseconds interval = std::chrono::milliseconds(100);  ///< Time interval between updates.

    protected:
        std::vector<std::unique_ptr<Device>> devices;  ///< Collection of managed devices.
        std::mutex mutex;           ///< Mutex for thread synchronization.
        std::condition_variable cv;  ///< Condition variable for thread coordination.

    private:
        /// Background thread for performing device updates.
        std::thread worker;

        /// Flag indicating whether the update thread is running.
        std::atomic<bool> isRunning = true;

        /// Worker function for the update thread.
        void work();
    };

    template <typename T>
    class Listener::iterator {
    public:
        iterator(const std::vector<std::unique_ptr<Device>>& devices, int index) : devices(&devices), index(index) {}

        bool operator!=(const iterator& other) const { return index != other.index; }
        bool operator==(const iterator& other) const { return index == other.index; }
        
        bool operator>(const iterator& other) const { return index > other.index; }
        bool operator<(const iterator& other) const { return index < other.index; }

        bool operator>=(const iterator& other) const { return index >= other.index; }
        bool operator<=(const iterator& other) const { return index <= other.index; }

        iterator& operator++() { return (++index, *this); }
        iterator& operator--() { return (--index, *this); }

        iterator& operator+=(int n) { return (index += n, *this); }
        iterator& operator-=(int n) { return (index -= n, *this); }

        iterator operator+(int n) const { return iterator(devices, index + n); }
        iterator operator-(int n) const { return iterator(devices, index - n); }

        int operator-(const iterator& other) const { return index - other.index; }

        T& operator*() const { 
            if (index < 0 || index >= (int) devices->size()) throw std::out_of_range("Index is out of range"); 
            return dynamic_cast<T&>(*devices->operator[](index)); 
        }

        template <typename U>
        operator iterator<U>() const { return iterator<U>(*devices, index); }
    
    private:
        const std::vector<std::unique_ptr<Device>>* devices;
        int index;
    };
}

namespace Project::nexus::abstract::c_wrapper { 
    class Listener : virtual public nexus::abstract::Listener {
    public:
        Listener() = default;
        virtual ~Listener();

        std::string path() const override;
        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;
        std::string patch(std::string_view json_request) override;

        abstract::Restful* restful;
        void* c_members;
    };
}
#else
#include <stdint.h>
#include <stddef.h>

/// Opaque handle representing a Nexus listener object.
typedef void* nexus_listener_t;

/// Create a new Listener.
/// @return A pointer to the newly created Listener object.
nexus_listener_t nexus_listener_new();

/// Create a new Listener object with overridden functions.
/// @param restful A pointer to the Restful override. Set to null as default
/// @return A pointer to the newly created Listener object.
nexus_listener_t nexus_listener_override_new(
    nexus_restful_t restful,
    void* members
);

/// @brief Delete a Listener object with overridden functions.
/// @param listener A pointer to a Listener object.
void nexus_listener_delete(nexus_listener_t listener);

/// Adds a device to a listener.
/// @param listener Handle to the Nexus listener object.
/// @param device Handle to the Nexus device object to add.
void nexus_listener_add(nexus_device_t listener, nexus_device_t device);

/// Removes a device from a listener.
/// @param listener Handle to the Nexus listener object.
/// @param index Index of the device to remove.
void nexus_listener_remove(nexus_device_t listener, int index);

/// Retrieves the number of devices in a listener.
/// @param listener Handle to the Nexus listener object.
/// @return Number of devices in the listener.
size_t nexus_listener_len(nexus_device_t listener);

/// Retrieves a device from a listener by index.
/// @param listener Handle to the Nexus listener object.
/// @param index Index of the device to retrieve.
/// @return Handle to the device, or NULL if not found.
nexus_device_t nexus_listener_get_device(nexus_device_t listener, int index);

#endif
#endif // PROJECT_NEXUS_ABSTRACT_LISTENER_H