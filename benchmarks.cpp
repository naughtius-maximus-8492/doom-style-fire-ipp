#include <array>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "src/doomASCIIFire.h"

TEST_CASE("Benchmarks")
{
    std::array widths = {144, 360, 480, 720, 1080, 1440, 2160};

    for (const int width : widths)
    {
        int height = width * (16.0F / 9.0F);
        doomASCIIFire* tempFire = new doomASCIIFire(width, height);

        BENCHMARK("Decay Step " + std::to_string(height) + "x" + std::to_string(width))
        {
            tempFire->decayFrame();
        };

        BENCHMARK("Get frame " + std::to_string(height) + "x" + std::to_string(width))
        {
            tempFire->updateFrame();
        };

        delete tempFire;
    }
};
