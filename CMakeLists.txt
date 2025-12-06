cmake_minimum_required(VERSION 3.25.1)
project(doomAsciiFire)

set(CMAKE_CXX_STANDARD 26)

find_package(TBB REQUIRED)
find_package(IPP REQUIRED)

include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.11.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(doomAsciiFire
        src/main.cpp
        src/doomASCIIFire.h
        src/doomASCIIFire.cpp
)

add_executable(benchmarks
        benchmarks.cpp
        src/doomASCIIFire.cpp
        src/doomASCIIFire.h
)

target_link_libraries(doomAsciiFire PRIVATE
        TBB::tbb
        IPP::ipps
)

target_link_libraries(benchmarks PRIVATE
        Catch2::Catch2WithMain
        TBB::tbb
        IPP::ipps
)
