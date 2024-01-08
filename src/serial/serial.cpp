#include "nexus/serial/serial.h"
#include "nexus/serial/c_wrapper.h"
#include "nexus/abstract/c_wrapper.h"
#include "nexus/tools/detect_virtual_comm.h"
#include "nexus/tools/json.h"
#include <future>
#include <fcntl.h>   /* File Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions */
#include <etl/keywords.h>

using namespace nexus;
using namespace std::literals;

serial::Serial::Serial(Args args) {
    reconnect(args);
    worker = std::thread(&Serial::work, this);
}

serial::Serial::~Serial() {
    disconnect();
    isRunning = false;
    worker.join();
}

fun serial::Serial::json() const -> std::string {
    return "{"
        "\"isConnected\": " + std::string(isConnected() ? "true": "false") +  
    "}";
}

fun serial::Serial::post(std::string_view method_name, std::string_view json_string) -> std::string {
    val json = tools::json_parse(json_string);
    
    if (method_name == "disconnect") {
        disconnect();
        return tools::json_response_status_success("Request to disconnect serial communication");
    }

    if (method_name == "reconnect") {
        val port = json["port"].to_string();
        val speed = json["speed"].to_string();
        val timeout = json["timeout"].to_int_or(args_.timeout.count());

        Args args = args_;
        args.port = port ? std::string(port.data(), port.len()) : args_.port;
        args.speed = speed == "B115200" ? B115200 : speed == "B9600" ? B9600 : speed == "B57600" ? B57600 : args_.speed;
        args.timeout = std::chrono::milliseconds(timeout);

        reconnect(args);
        return tools::json_response_status_success("Request to reconnect serial communication");
    }

    if (method_name == "send") {
        val text = json["text"];

        if (not text.is_string())
            return tools::json_response_status_fail_mismatch_value_type();

        val sv = text.to_string();
        val res = send(std::string_view(sv.data(), sv.len()));
        return tools::json_concat(
            tools::json_response_status_success("Request to send"),
            "{\"res\": " + std::to_string(res) + "}"
        );
    }

    if (method_name == "receive_text") {
        val timeout = json["timeout"];
        std::string msg;

        if (not timeout) {
            msg = receiveText();
        } elif (timeout.is_number()) {
            msg = receiveText(std::chrono::milliseconds((int64_t) timeout.to_float()));
        } else {
            return tools::json_response_status_fail_mismatch_value_type();
        }
        
        return tools::json_concat(
            tools::json_response_status_success("Request to receive text"),
            "{\"message\": " + msg + "}"
        );
    }

    if (method_name == "receive_bytes") {
        val timeout = json["timeout"];
        std::vector<uint8_t> msg;

        if (not timeout) {
            msg = receiveBytes();
        } elif (timeout.is_number()) {
            msg = receiveBytes(std::chrono::milliseconds((int64_t) timeout.to_float()));
        } else {
            return tools::json_response_status_fail_mismatch_value_type();
        }

        std::string msg_str = "[";
        for (val byte in msg) {
            msg_str += std::to_string(byte) + ",";
        }
        msg_str.back() = ']';
        
        return tools::json_concat(
            tools::json_response_status_success("Request to receive bytes"),
            "{\"message\": " + msg_str + "}"
        );
    }

    return tools::json_response_status_fail_unknown_method();
}

fun serial::Serial::reconnect(Args args) -> void {
    std::scoped_lock lock1(txMutex);
    std::scoped_lock lock2(rxMutex);
    
    args_ = args;

    if (isConnected()) {
        disconnect();
    }

    var ports = std::vector<std::string>();
    if (args_.port == null or args_.port == std::string("auto")) {
        ports = tools::detectVirtualComm();
        if (not ports.empty())
            args_.port = ports[0];
    }

    fd = open(args_.port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
        return;

    struct termios tty = {};
    if (tcgetattr(fd, &tty) != 0) {
        disconnect();
        return;
    }

    cfsetospeed(&tty, args_.speed);
    cfsetispeed(&tty, args_.speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 /* 8-bit characters */
    tty.c_cflag &= ~(PARENB | PARODD);  /* no parity bit */
    tty.c_cflag &= ~CSTOPB;             /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;            /* no hardware flow control */

    // /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | IXOFF | IXON | IXANY | INLCR | ICRNL);
    tty.c_lflag = 0; // no signaling chars, no echo, no canonical processing
    tty.c_oflag = ~OPOST; // no remapping

    int vtime = args_.timeout.count() / 100;
    vtime = etl::clamp(vtime, 0, 10);

    tty.c_cc[VTIME] = (cc_t) (vtime); 
    tty.c_cc[VMIN]  = 0; 

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        disconnect();
        return;
    }

    // clear rx buffer
    tcflush(fd, TCIOFLUSH);
}

fun serial::Serial::reconnect() -> void {
    reconnect(args_);
}

fun serial::Serial::disconnect() -> void {
    if (isConnected()) {
        close(fd);
        fd = -1;
    }
}

fun serial::Serial::isConnected() const -> bool {
    return fd >= 0;
}

fun serial::Serial::send(nexus::byte_view buffer) -> int {
    if (not isConnected()) 
        return -1;

    std::scoped_lock lock(txMutex);
    auto buf = encode(buffer);
    val res = write(fd, buf.data(), buf.size());
    if (res < 0)
        disconnect();

    return (int) res;
}

fun serial::Serial::send(std::string_view text) -> int {
    return send({reinterpret_cast<const uint8_t*>(text.data()), text.size()});
}

fun serial::Serial::receiveBytes(std::function<bool(nexus::byte_view)> filter, std::chrono::milliseconds timeout) -> std::vector<uint8_t> {
    if (not isConnected()) 
        return null;
        
    std::unique_lock<std::mutex> lock(receivedMessageMutex);
    return receivedMessageCondition.wait_for(lock, timeout, [this, fn=std::move(filter)] { 
        return not isRunning or (not receivedMessage.empty() and fn({receivedMessage.data(), receivedMessage.size()})); 
    }) and isRunning ? std::move(receivedMessage) : null;
}

fun serial::Serial::receiveBytes(std::chrono::milliseconds timeout) -> std::vector<uint8_t> {
    return receiveBytes([] (nexus::byte_view) { return true; }, timeout);
}

fun serial::Serial::receiveText(std::function<bool(std::string_view)> filter, std::chrono::milliseconds timeout) -> std::string {
    val buf = receiveBytes([fn=std::move(filter)] (nexus::byte_view received) { 
        return fn({reinterpret_cast<const char*>(received.data()), received.len()}); 
    }, timeout);
    return std::string(buf.begin(), buf.end());
}

fun serial::Serial::receiveText(std::chrono::milliseconds timeout) -> std::string {
    return receiveText([] (std::string_view) { return true; }, timeout);
}

fun serial::Serial::addCallback(std::function<void(nexus::byte_view)> fn) -> void {
    callbacks.push_back(std::move(fn));
}

fun serial::Serial::encode(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    return std::vector<uint8_t>(buffer.begin(), buffer.end());
}

fun serial::Serial::decode(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    return buffer.len() == 0 or buffer.back() != '\n' 
        ? std::vector<uint8_t>()
        : std::vector<uint8_t>(buffer.begin(), buffer.end());
}

fun serial::Serial::work() -> void {
    size_t rxIndex = 0;
    val launch_callback = [this] {
        if (not callbacks.empty())
            std::ignore = std::async(std::launch::async, [this, msg=receivedMessage] {
                for (var &callback in callbacks) {
                    callback({msg.data(), msg.size()});
                }
            });
    };
            
    val try_decode = [&, this] {
        for (val i in etl::range(rxIndex + 1)) {
            val len = rxIndex + 1 - i;
            val data = decode({rxBuffer + i, len});

            if (not data.empty()) {
                std::lock_guard<std::mutex> lock(receivedMessageMutex);
                receivedMessage = std::move(data);
            } else {
                continue;
            }

            launch_callback();
            receivedMessageCondition.notify_all();
            rxIndex = 0;
            return;
        }

        rxIndex++;
        rxIndex %= sizeof(rxBuffer);
    };

    val try_read = [&, this] {
        std::scoped_lock lock(rxMutex);
        return read(fd, rxBuffer + rxIndex, 1);
    };

    while (isRunning) {
        if (not isConnected()) {
            std::this_thread::sleep_for(1ms);
            continue;
        }

        val n = try_read();

        // success
        if (n > 0) 
            try_decode();
        
        // timeout
        elif (n == 0)
            continue;

        // disconnected
        else 
            disconnect();
    }
}
