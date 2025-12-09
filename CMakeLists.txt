cmake_minimum_required(VERSION 3.25.1)
project(doomAsciiFire)

set(CMAKE_CXX_STANDARD 23)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(TBB REQUIRED)
find_package(IPP REQUIRED)

include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.11.0
)

FetchContent_MakeAvailable(Catch2)

set(SOURCES
        src/doomASCIIFire.h
        src/doomASCIIFire.cpp
        src/utils.h
        src/linux-keys.cpp
        src/linux-keys.h
        src/linux-virtual-keys.h
        src/keys.h
)

add_executable(doomAsciiFire
        src/main.cpp
        ${SOURCES}
)

add_executable(benchmarks
        ${SOURCES}
        benchmarks.cpp
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
