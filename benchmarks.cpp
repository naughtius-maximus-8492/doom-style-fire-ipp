#include <array>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "src/doomASCIIFire.h"

TEST_CASE("Secret Entrance One")
{
    std::array<int, 10> sizes = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    for (int size : sizes)
    {
        BENCHMARK("Decay Step" + std::to_string(size) + "x" + std::to_string(size))
        {
            const doomASCIIFire fire(size, size);
            fire.decayStep();
        };

    }
}
