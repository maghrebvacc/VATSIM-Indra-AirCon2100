# Indra Plugin — EuroScope SDD Workflow Plugin

## Prerequisites
- EuroScope SDK (set path in CMakeLists.txt)
- nlohmann/json (header-only, place json.hpp in a location on your include path)
- Windows SDK
- CMake 3.10+
- MSVC or MinGW (Windows)

## Build
```
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A Win32
cmake --build . --config Release
```

