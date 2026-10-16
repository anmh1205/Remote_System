# Remote System

A Qt 6 desktop application for remote device control, network configuration, and MJPEG camera streaming, built with C++17 and CMake.

## Overview

Remote System brings device-management controls and live camera viewing into a native desktop interface. The application uses Qt Widgets for its UI, Qt Network for communication, and dedicated controller, model, and camera components to separate interface behavior from network and streaming logic.

The repository contains the application source code and Qt Designer UI files. It is an application project, not a reusable remote-control SDK.

## Main components

| Component | Responsibility |
| --- | --- |
| `MainWindow` and views | Qt window and pages for control, Wi-Fi information, and IP configuration |
| `MainController` | Coordinates UI actions and supporting controllers |
| `NetworkController`, `TcpClient`, `TcpServer` | Device communications and TCP networking |
| `CameraController`, `CameraManager` | Camera connections and stream management |
| `MjpegStreamHandler` | MJPEG stream processing |
| Models | Device, connection, network, and Wi-Fi state |

The code structure supports both configuration and video-related workflows, but successful operation depends on the external device and its compatible network endpoints.

## Requirements

- A C++17-capable compiler.
- CMake 3.16 or newer.
- Qt 6 development packages for `Core`, `Widgets`, `Network`, `Multimedia`, and `MultimediaWidgets`.
- A device/network environment compatible with the application's configuration and camera protocols.

## Build

Install Qt 6 and CMake, then run from the repository root:

```sh
cmake -S . -B build
cmake --build build --config Release
```

If Qt is installed in a nonstandard location, configure CMake's `CMAKE_PREFIX_PATH` to point to the Qt installation. The executable target is named `RemoteSystem`.

On Windows, the project requests a GUI executable. On other supported Qt development platforms, available compiler and Qt configurations may differ. No automated build validation is asserted by this README.

## Project structure

- [`src/main.cpp`](src/main.cpp) — application entry point.
- [`src/MainWindow.cpp`](src/MainWindow.cpp) — main window.
- [`src/controllers/`](src/controllers/) — UI and device control flow.
- [`src/models/`](src/models/) — state models.
- [`src/network/`](src/network/) — TCP clients, server, and network helpers.
- [`src/camera/`](src/camera/) — camera manager and MJPEG handling.
- [`src/views/`](src/views/) and [`ui/`](ui/) — Qt Widgets views and Designer forms.
- [`CMakeLists.txt`](CMakeLists.txt) — build configuration.

## Development status and limitations

This is a source-level desktop application. The repository does not currently supply a packaged installer, deployment manual for the target hardware, or tested platform matrix. Connection settings and streaming behavior need to be verified against the intended device.

No repository-wide license is currently declared.
