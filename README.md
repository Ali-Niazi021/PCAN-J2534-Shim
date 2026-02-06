# PCAN-J2534-Shim

A minimal J2534 shim DLL for the PEAK PCAN-USB interface that intercepts `PassThruIoctl` `READ_VBATT` requests and returns a spoofed 13.5 V (13 500 mV) value, while passing every other J2534 v04.04 API call straight through to the real PEAK `PCANPT32.dll`.

## Background

Some OEM dealer diagnostic tools (e.g. Subaru SSM4) call `PassThruIoctl(READ_VBATT)` with an incorrect handle — passing the **DeviceID** instead of the **ChannelID**. The PCAN-USB firmware rejects the call, which causes the tool to abort with a voltage-check error even though the vehicle battery is fine.

This shim sits between the application and the real PCAN driver and short-circuits the `READ_VBATT` request so the tool never sees the error.

## Based On

This project is based on [joeyoravec/j2534-logger](https://github.com/joeyoravec/j2534-logger), a debugging/logging shim library for PassThru Vehicle Reprogramming (SAE J2534). The original project's DLL-loading and function-forwarding architecture was adapted into this minimal VBATT shim.

## How It Works

1. `ptshim32.dll` is registered in the Windows registry as a J2534 PassThru device.
2. When a J2534 application loads it, the shim lazily loads the real `PCANPT32.dll` from the same directory on the first API call.
3. All 14 standard J2534 v04.04 functions are forwarded to the real DLL unchanged — **except** `PassThruIoctl`:
   - If `IoctlID == READ_VBATT`, the shim writes `13500` (mV) to the output buffer and returns `STATUS_NOERROR` immediately, without calling the real driver.
   - All other IOCTL commands are forwarded normally.

## Project Structure

```
ptshim32/
  dllmain.cpp           – DLL entry point
  shim_frontend.cpp/h   – Exported J2534 API functions (VBATT intercept lives here)
  shim_loader.cpp/h     – Loads PCANPT32.dll and resolves function pointers
  j2534_v0404.h         – J2534 v04.04 type definitions
  ptshim32.def          – Module-definition file (ordinal exports)
  ptshim32.vcxproj      – Visual Studio project
pcan-dll/               – Reference copies of the PCAN PassThru DLLs
installer/              – WiX-based MSI installer project
install_shim.reg        – Sample registry file to register the shim
```

## Building

1. Open `j2534-logger.sln` in Visual Studio (2017 or later).
2. Select the **Release | x86** configuration.
3. Build the `ptshim32` project. The output DLL is `Release\ptshim32.dll`.

> **Note:** The shim must be built as a **32-bit** DLL to match the PCAN PassThru driver.

## Installation

### Option A — Registry file

1. Copy `ptshim32.dll` to the PCAN PassThru directory (e.g. `C:\Program Files\PEAK-System\PCAN-PassThru API\04.04\32\`).
2. Double-click `install_shim.reg` to register the shim as a J2534 device.

### Option B — MSI installer

Build and run the WiX installer project under `installer/`.

### Registry entry

The registry key created under `HKEY_LOCAL_MACHINE\SOFTWARE\PassThruSupport.04.04\PCAN-USB VBATT Shim` tells J2534 applications to load `ptshim32.dll` instead of the stock PEAK DLL. The shim then loads the real `PCANPT32.dll` from the same directory transparently.

## Supported Protocols

| Protocol  | Supported |
|-----------|-----------|
| CAN       | Yes       |
| ISO 15765 | Yes       |
| ISO 14230 | Yes       |
| ISO 9141  | Yes       |
| J1850 PWM | No        |
| J1850 VPW | No        |

## License

This project is licensed under the [GNU General Public License v3.0](LICENSE).
