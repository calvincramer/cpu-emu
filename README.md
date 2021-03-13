# Cpu emulation

* MOS 6502


# Development Instructions

* Develeoped on Ubuntu 20.04 (WSL2)
* Packages:
```sh
# g++ compiler
sudo apt install build-essential

# cmake
sudo apt-get install cmake

# google test
sudo apt-get install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp lib/*.a /usr/lib

# Python3
# Under /usr/bin/python3
# Used python3 3.8.5. Make work with older versions, not tested though.
```
