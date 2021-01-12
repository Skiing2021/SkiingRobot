# Skiing Robot main program

This program is designed for Sipeed K210 board

## Features

- [ ] Skiing gate detect using tiny yolo
- [ ] Robot automatic control

## Getting started

#### Requirements

- CMake 
- CLion
- [Standalone SDK for kendryte K210](https://github.com/kendryte/kendryte-standalone-sdk)

- RISC-V build toolchain

#### Build

1. Clone repo `https://github.com/Skiing2021/SkiingRobot.git`

2. Use CLion to open `Robot` directory

3. Set environment variable `SDK_ROOT` to K210 standalone SDK root path

4. Set environment variable `TOOLCHAIN` to RISC-V build toolchain path

   ###### For Windows:

   a. Download and extract [kendryte-toolchain-win-i386-8.2.0-20190409.tar.xz](https://github.com/kendryte/kendryte-gnu-toolchain/releases/download/v8.2.0-20190409/kendryte-toolchain-win-i386-8.2.0-20190409.tar.xz) 

   b. Set environment variable `TOOLCHAIN` to `<parent_dir>/kendryte-toolchain/bin`

   

