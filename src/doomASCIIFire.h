#pragma once

#include <ipp.h>
#include <tbb/tbb.h>
#include <string>
#include <thread>
#include <iostream>
#include <random>
#include "utils.h"

constexpr unsigned int minIntensity = 0;
constexpr unsigned int maxIntensity = 762;

constexpr int defaultLowBoundUniform = 0;
constexpr int defaultUpperBoundUniform = 75;
constexpr int defaultMeanGauss = -5;
constexpr int defaultStandardDeviationGauss = 25;

constexpr unsigned int defaultDelay = 62;
constexpr unsigned int maxCharacterSize = 52;

constexpr int flicker = 2;

class doomASCIIFire
{
private:
     std::string characters;
     int frameBufferWidth;
     int frameBufferHeight;
     int frameBufferSize;
     // int frameBufferFullSize;
     int frameBufferTopSize;

     // int frameBufferPadding;
     // Ipp16s* frameBufferStart;
     Ipp16s* frameBuffer;
     Ipp16s* uniformRandomBuffer;
     Ipp16s* gaussianRandomBuffer;
     IppsRandUniState_16s* uniformRandomState;
     IppsRandGaussState_16s* gaussianRandomState;

     void initRandomFunctions();

     char intensityToChar(int intensity) const;
     std::string intensityToColour(int intensity) const;
     std::string getCharacter(int intensity) const;

     static float normalise(float value, float min, float max);

     time_t seededTime;

public:
     bool running;
     float colour_band_multiplier;
     bool backgroundMode;
     int frameDelay;

     doomASCIIFire(int width, int height);
     ~doomASCIIFire();

     std::string getFrame() const;

     void decayStep() const;

     void openConfig();

     void updateDecayRate(short decayRate) const;
};
