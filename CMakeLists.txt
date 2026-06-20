cmake_minimum_required(VERSION 3.10)
project(IndraPlugin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# winsock2.h included via stdafx.h before windows.h - no extra defines needed
include_directories(include src)

# Folder containing EuroScopePlugIn.lib
link_directories(${CMAKE_SOURCE_DIR}/lib)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(IndraPlugin SHARED ${SOURCES}
        src/plugin.cpp
        src/common/buttons.cpp
        src/common/buttons.h
        src/common/bar.cpp
        src/common/bar.h
        src/FlighPlan/GetFlightPlanData.cpp
        src/FlighPlan/DrawFlightPlanWindow.cpp
        src/FlighPlan/GetFlightPlanData.h
        src/FlighPlan/DrawFlightPlanWindow.h)

target_link_libraries(IndraPlugin ws2_32 comctl32 EuroScopePlugInDll)

set_target_properties(IndraPlugin PROPERTIES
        PREFIX ""
        SUFFIX ".dll"
        # Only the .dll lands directly in the build/<config> output folder.
        # The import .lib/.exp and debug files are pushed into a side folder
        # so they don't clutter the main output directory.
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/intermediate/$<CONFIG>"
        PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/intermediate/$<CONFIG>"
)