#include <array>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "src/doomASCIIFire.h"

TEST_CASE("Secret Entrance One")
{
    std::array<int, 8> sizes = {64, 128, 256, 512, 1024, 2048, 4096, 8192};
    for (int size : sizes)
    {
        const doomASCIIFire fire(size, size);
        BENCHMARK("Decay Step" + std::to_string(size) + "x" + std::to_string(size))
        {
            fire.decayStep();
        };

        BENCHMARK("Get frame" + std::to_string(size) + "x" + std::to_string(size))
        {
            std::string frame = fire.getFrame();
        };
    }
}
