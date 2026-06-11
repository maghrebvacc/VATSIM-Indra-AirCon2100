cmake_minimum_required(VERSION 4.2)
project(Indra_Plugin_VATSIM)

set(CMAKE_CXX_STANDARD 20)

add_executable(Indra_Plugin_VATSIM main.cpp
        src/Storage.cpp)
