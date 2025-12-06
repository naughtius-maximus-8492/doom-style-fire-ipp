#pragma once

#include <ipp.h>
#include <tbb/tbb.h>
#include <string>
#include <thread>
#include <iostream>
#define NOMINMAX
#include <Windows.h>
#include <random>

constexpr unsigned int minIntensity = 0;
constexpr unsigned int maxIntensity = 254;

constexpr int fixedLowBoundUniform = 0;
constexpr int fixedMeanGauss = 0;

constexpr unsigned int defaultDelay = 33;
constexpr unsigned int maxCharacterSize = 52;

constexpr int flicker = 3;

class doomASCIIFire
{
private:
     std::string characters;
     unsigned int frameBufferWidth;
     unsigned int frameBufferHeight;
     int frameBufferSize;
     unsigned int frameBufferTopSize;

     unsigned int frameBufferPadding;
     Ipp16s* frameBufferStart;
     Ipp16s* frameBuffer;
     Ipp16s* uniformRandomBuffer;
     Ipp16s* gaussianRandomBuffer;
     IppsRandUniState_16s* uniformRandomState;
     IppsRandGaussState_16s* gaussianRandomState;

     void initRandomFunctions();

     char intensityToChar(int intensity) const;
     std::string intensityToColour(int intensity) const;

     static float normalise(float value, float min, float max);

     time_t seededTime;

public:
     doomASCIIFire(int width, int height);
     ~doomASCIIFire();

     std::string getFrame() const;
     void decayStep() const;

     void openConfig();

     void updateDecayRate(int decayRate) const;

     float colour_band_multiplier;
     bool backgroundMode;
     int frameDelay;
};