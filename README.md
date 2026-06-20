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

## Structure
- `src/plugin/`       — Main plugin entry point
- `src/integration/`  — EuroScope & TopSky API wrappers
- `src/data/`         — Data aggregation and filtering
- `src/workflow/`     — Sequence, time, coordination, route modules
- `src/ui/`           — Toolbar and list window managers
- `src/config/`       — Settings and profile management
- `src/util/`         — Logger
