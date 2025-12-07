#include <array>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "src/doomASCIIFire.h"

TEST_CASE("Benchmarks")
{

    doomASCIIFire genericFire(256, 256);

    std::array<int, 8> widths = {144, 360, 480, 720, 1080, 1440, 2160};
    for (const int width : widths)
    {
        int height = width * (16.0F / 9.0F);
        const doomASCIIFire tempFire(width, height);

        BENCHMARK("Decay Step " + std::to_string(height) + "x" + std::to_string(width))
        {
            tempFire.decayStep();
        };

        BENCHMARK("Get frame " + std::to_string(height) + "x" + std::to_string(width))
        {
            std::string frame = tempFire.getFrame();
        };
    }

    BENCHMARK("UpdateDecay Rate")
    {
        genericFire.updateDecayRate(100);
    };

}
