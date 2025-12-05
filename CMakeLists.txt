cmake_minimum_required(VERSION 4.0)
project(doomAsciiFire)

set(CMAKE_CXX_STANDARD 26)

find_package(TBB REQUIRED)
find_package(IPP REQUIRED)

add_executable(doomAsciiFire
        main.cpp
        doomASCIIFire.h
        doomASCIIFire.cpp
)

target_link_libraries(doomAsciiFire TBB::tbb IPP::ipps)
