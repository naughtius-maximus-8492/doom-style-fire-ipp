cmake_minimum_required(VERSION 3.25.1)
project(doomAsciiFire)

set(CMAKE_CXX_STANDARD 23)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(TBB REQUIRED)
find_package(IPP REQUIRED)

include(external_libs/CPM.cmake)
CPMAddPackage(gh:catchorg/Catch2@3.15.0)

set(SOURCES
        src/doomASCIIFire.h
        src/doomASCIIFire.cpp
        src/utils.h
        src/linux-keys.cpp
        src/linux-keys.h
        src/linux-virtual-keys.h
        src/keys.h
)

set(INCLUDES
        external_libs/perlin-noise
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

target_include_directories(benchmarks PRIVATE
        ${INCLUDES}
)

target_include_directories(doomAsciiFire PRIVATE
        ${INCLUDES}
)
