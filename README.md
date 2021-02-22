# Cpu emulation

* MOS 6502


# Development Instructions

* Develeoped on Ubuntu 20.04 (WSL2)
* Packages:
```sh
# cmake
sudo apt-get install cmake

# google test
sudo apt-get install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp lib/*.a /usr/lib
```
