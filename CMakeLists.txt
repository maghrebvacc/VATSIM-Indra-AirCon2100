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
        src/FlighPlan/Get/GetFlightPlanData.cpp
        src/FlighPlan/DrawWindow/DrawFlightPlanWindow.cpp
        src/FlighPlan/Get/GetFlightPlanData.h
        src/FlighPlan/DrawWindow/DrawFlightPlanWindow.h
        src/FlighPlan/Modify/ModifyFlightPlan.cpp
        src/FlighPlan/Modify/ModifyFlightPlan.h
        src/Bright/DrawBrightWindow.cpp
        src/Bright/DrawBrightWindow.h
        src/FlighPlan/DrawWindow/View/DrawViewFlightPlanWindow.cpp
        src/FlighPlan/DrawWindow/View/DrawViewFlightPlanWindow.h
        src/FlighPlan/DrawWindow/Modify/DrawModifyFlightPlanWindow.cpp
        src/FlighPlan/DrawWindow/Modify/DrawModifyFlightPlanWindow.h)

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