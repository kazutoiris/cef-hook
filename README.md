# CefHook

`CefHook` is an example project that demonstrates how to hook functions in applications based on the [Chromium Embedded Framework (CEF)](https://bitbucket.org/chromiumembedded/cef/).

## Features

This project hooks the following functions:

1. `on_key_event` and `on_load_end` events in `libcef.dll`.
2. The `SetWindowDisplayAffinity` function in `user32.dll`.

## How to Build

The project is organized using [CMake](https://cmake.org/), and it can be compiled using various build tools. Below are the options for compiling the project:

1. **Visual Studio (2019 or newer)**

    Simply open the project folder in Visual Studio, and it will automatically detect the CMake configuration. You can then build the solution as usual.

2. **Vanilla**

    You can also build the project directly from the command line. After configuring CMake, you can use your preferred build system to compile the project.

## Dependencies

- **vcpkg**: A package manager for C and C++ libraries, used for managing third-party dependencies. It simplifies the process of acquiring libraries such as CEF and Detours. You can install it from the [microsoft/vcpkg](https://github.com/microsoft/vcpkg).
  - **Detours** (Automatically downloaded by vcpkg): A library for intercepting and modifying function calls, which is used for hooking. You can find more information on the [microsoft/Detours](https://github.com/microsoft/Detours).
- **Visual Studio** / **Microsoft Visual C++ Build Tools**: Recommended for Windows-based development.
  - **CMake** (Bundled with Visual Studio): A tool for configuring and managing the build process of the project.
- **Chromium Embedded Framework (CEF)** (Embedded as a submodule): CEF must be available in your development environment. Ensure that the CEF binaries and include files are correctly configured.

## License

This project is licensed under the Anti-996 License. See the [LICENSE](LICENSE) file for more details.