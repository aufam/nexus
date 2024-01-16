# Nexus Library
Nexus is a C++ library designed to streamline the communication and interaction with hardware devices 
through various protocols. It provides a modular and extensible framework for creating RESTful interfaces, 
managing devices, handling communication protocols, and supporting a listener pattern for periodic updates.

## Features
- **[Restful](include/nexus/abstract/restful.h):** 
Define objects that can be converted to JSON and support common HTTP methods (GET, POST, PATCH).
- **[Device](include/nexus/abstract/device.h):** 
Represent various hardware devices, inheriting from Restful for easy integration with RESTful interfaces.
- **[Communication](include/nexus/abstract/communication.h):** 
Implement communication protocols such as [serial](include/nexus/serial), 
[TCP](include/nexus/tcp), [HTTP](include/nexus/http), [Modbus RTU](include/nexus/modbus/rtu/), and [Modbus TCP](include/nexus/modbus/tcp/).
- **[Listener](include/nexus/abstract/listener.h):** 
Manage multiple devices within a separate thread, allowing for periodic updates.

## Table of Contents
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Build python module](#python_module)
- [Build tests](#python_module)
- [Build documentation](#docs)
- [How to use](#how_to_use)
- [Example usage](#example_usage)

## <a id="prerequisites"></a>Prerequisites
* cmake minimum version 3.14 (required)
  ```bash
  sudo apt install cmake
  ```
* [Embedded Template Library (ETL)](https://github.com/aufam/etl) (required)
  ```bash
  git clone https://github.com/aufam/etl
  cd etl
  mkdir build
  cmake -B build
  make -C build
  sudo make install -C build
  ```
* [httplib](https://github.com/yhirose/cpp-httplib) (required)
  ```bash
  git clone https://github.com/yhirose/cpp-httplib
  cd cpp-httplib
  mkdir build
  cmake -B build
  make -C build
  sudo make install -C build
  ```
* [pybind11](https://github.com/pybind/pybind11) (required for building [python module](#python_module))
  ```bash
  pip install pybind11
  ```
* [googletest](https://github.com/google/googletest) (required for building [tests](#tests))
  ```bash
  git clone https://github.com/google/googletest
  cd googletest
  mkdir build
  cmake -B build
  make -C build
  sudo make install -C build
  ```
* [Doxygen](https://github.com/doxygen/doxygen) (required for building [docs](#docs))
  ```bash
  git clone https://github.com/doxygen/doxygen
  cd doxygen
  mkdir build
  cmake -B build
  make -C build
  sudo make install -C build
  ```

## <a id="installation"></a>Installation
```bash
git clone https://github.com/aufam/nexus.git
cd nexus
mkdir build
cmake -B build
make -C build
sudo make install -C build
```

To uninstall:
```bash
sudo make uninstall -C build
```

## <a id="python_module"></a>Build python module
Prerequisites:
* [pybind11](https://github.com/pybind/pybind11.git)
```bash
cmake -DBUILD_PYBIND=ON -B build
make -C build
sudo make install -C build
```

You can specify the installation location for the Python module by specifying the CMake variable `PYTHON_PACKAGES_DIR`. 
For example:
```bash
cmake -DBUILD_PYBIND=ON -DPYTHON_PACKAGES_DIR=/path/to/installation/directory -B build
```

You also can specify the `pybind11-config` executable by specifying the CMake variable `PYBIND_CONFIG`. 
For example:
```bash
cmake -DBUILD_PYBIND=ON -DPYBIND_CONFIG=/path/to/pybind11-config -B build
```

## <a id="tests"></a>Build tests
Prerequisites:
* [googletest](https://github.com/google/googletest)
```bash
cmake -DBUILD_TESTS=ON -B build
make -C build
sudo make install -C build
```

run:
```bash
./build/test/test_all
```

## <a id="docs"></a>Build documentation
Prerequisites:
* [Doxygen](https://github.com/doxygen/doxygen.git)
```bash
cmake -DBUILD_DOCS=ON -B build
make -C build
```
Documentation will be generated in
[build/docs/html/index.html](build/docs/html/index.html)

## <a id="how_to_use"></a>How to use
### C/C++
Import to your cmake project by adding:
```cmake
find_package(nexus CONFIG REQUIRED)
target_link_libraries(<your_project> PUBLIC nexus)
```
For makefile 
(you have to specify the variables according to your project):
```Makefile
LIB += -lnexus -pthread
```

### Python
Install the [python module](#python_module)

## <a id="example_usage"></a>Example usage
See [apps](apps) for some implementation examples