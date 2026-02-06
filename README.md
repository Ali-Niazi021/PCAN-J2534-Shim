# PCAN-J2534-Shim

A minimal J2534 shim DLL for the PEAK PCAN-USB interface that intercepts `PassThruIoctl` `READ_VBATT` requests and returns a spoofed 13.5 V (13 500 mV) value, while passing every other J2534 v04.04 API call straight through to the real PEAK `PCANPT32.dll`.

## Background

Some OEM dealer diagnostic tools (e.g. Subaru SSM4) call `PassThruIoctl(READ_VBATT)` which the PCAN does not support, thus they do not get a voltage reading and usually cease to function after this point. Tested with SSM4 to be working. 

This shim sits between the application and the real PCAN driver and short-circuits the `READ_VBATT` request so the tool never sees the error.

## Based On

This project is based on [joeyoravec/j2534-logger](https://github.com/joeyoravec/j2534-logger), a debugging/logging shim library for PassThru Vehicle Reprogramming (SAE J2534). The original project's DLL-loading and function-forwarding architecture was adapted into this minimal VBATT shim.

## How It Works

1. `ptshim32.dll` is registered in the Windows registry as a J2534 PassThru device.
2. When a J2534 application loads it, the shim lazily loads the real `PCANPT32.dll` from the same directory on the first API call.
3. All 14 standard J2534 v04.04 functions are forwarded to the real DLL unchanged — **except** `PassThruIoctl`:
   - If `IoctlID == READ_VBATT`, the shim writes `13500` (mV) to the output buffer and returns `STATUS_NOERROR` immediately, without calling the real driver.
   - All other IOCTL commands are forwarded normally.

## Building

1. Open `j2534-logger.sln` in Visual Studio (2017 or later).
2. Select the **Release | x86** configuration.
3. Build the `ptshim32` project. The output DLL is `Release\ptshim32.dll`.

> **Note:** The shim must be built as a **32-bit** DLL to match the PCAN PassThru driver.

## Installation

### Registry file

1. Copy `ptshim32.dll` to the PCAN PassThru directory (e.g. `C:\Program Files\PEAK-System\PCAN-PassThru API\04.04\32\`).
2. Double-click `install_shim.reg` to register the shim as a J2534 device.

### Registry entry

The registry key created under `HKEY_LOCAL_MACHINE\SOFTWARE\PassThruSupport.04.04\PCAN-USB VBATT Shim` tells J2534 applications to load `ptshim32.dll` instead of the stock PEAK DLL. The shim then loads the real `PCANPT32.dll` from the same directory transparently.
