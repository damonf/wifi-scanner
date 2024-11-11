# Wifi Scanner

This program scans for wireless access points.  
It uses the [libnl](https://www.infradead.org/~tgr/libnl/) library to communicate with the Linux kernel via the [Generic Netlink](https://kernel.org/doc/html/latest/userspace-api/netlink/intro.html#generic-netlink) protocol, specifically to gather wireless network information.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

To build and work with this project, ensure the following tools and libraries are installed on your system:

1. **GNU g++ Compiler**  
   This project requires a compiler that supports C++23, specifically for the `std::expected` feature, which is used in the codebase. Ensure you have a version of `g++` that includes `std::expected` support. You can check the [GCC C++23 status page](https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html#status.iso.2023) to confirm compatibility.

2. **[Ninja Build System](https://ninja-build.org/)**  
   Install `Ninja` on Ubuntu with:
   ```bash
   sudo apt install ninja-build
   ```

3. Clang Tools (clang-format and clang-tidy)
   These tools from the LLVM compiler collection are used for code formatting and static analysis.
   To install Clang on Ubuntu, add the LLVM package repository and install:
   ```bash
   # Add the LLVM repository
   sudo apt install clang-format clang-tidy
   ```

4. libnl Libraries
   This project requires libnl libraries for network communication with the kernel.
   Install the libnl development libraries on Ubuntu with:
   ```bash
   sudo apt install libnl-3-dev libnl-genl-3-dev
   ```


### Building the WiFi Scanner Project

This repository is using [Git submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) to manage dependencies.

#### Cloning the Repository with Submodules

To clone the repository and initialize submodules in one step:
```bash
git clone --recurse-submodules https://github.com/damonf/wifi-scanner.git
```

For a faster clone:
```bash
git clone --recurse-submodules --depth 1 --shallow-submodules https://github.com/damonf/wifi-scanner.git
```

#### Adding Submodules to an Existing Clone

If you cloned the repository without submodules, you can initialize them afterward with:
```bash
git submodule update --init --recursive --depth 1
```
#### Building the Project
After cloning and initializing submodules, you can build everything, including dependencies, by running:
```bash
./task.sh
```
This script will handle the entire build process, making sure all dependencies are correctly configured.

#### Running the program
After a successfull build the program can be run with:
```
sudo ./build/apps/wifi-scanner <wifi_interface>
```
- Replace <wifi_interface> with the name of the wireless network interface you want to scan (e.g., wlan0).
- The program requires root privileges to access the network interface, which is why it must be run with sudo.

## Running the tests

Unit tests can be run with:
```
./task.sh test
```

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
