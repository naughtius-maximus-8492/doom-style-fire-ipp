#pragma once

#include <ipp.h>
#include <tbb/tbb.h>
#include <string>
#include <thread>
#include <iostream>
#include <random>
#include "utils.h"

constexpr unsigned int minIntensity = 0;
constexpr unsigned int maxIntensity = 16384;

constexpr int defaultLowBoundUniform = 0;
constexpr int defaultMeanGauss = -5;

constexpr unsigned int defaultDelay = 67;
constexpr unsigned int fixedCharacterLength = 42;

constexpr int defaultFlicker = 4;

constexpr const char *defaultFlameGradient = " `'^x!|ioedO0&#";

class doomASCIIFire
{
private:
     int decayRate;
     std::string characters;
     int frameBufferWidth;
     int frameBufferHeight;
     int frameBufferSize;

     Ipp16s* frameBuffer;

     Ipp16s* gaussRandomBuffer;
     Ipp16s* uniformRandomBuffer;
     IppsRandUniState_16s* uniformRandomState;
     IppsRandGaussState_16s* gaussianRandomState;

     void initRandomFunctions();

     char intensityToChar(int intensity) const;
     std::string intensityToColour(int intensity) const;
     void setCharacter(int intensity, char* frameBufPos, bool newline) const;

     static float normalise(float value, float min, float max);

     time_t seededTime;

public:
     char* charFrameBuffer;
     float colour_band_multiplier;
     bool backgroundMode;
     int frameDelay;
     int flicker;

     doomASCIIFire(int width, int height);
     ~doomASCIIFire();

     void updateFrame() const;

     void decayStep() const;

     void openConfig();

     void updateDecayRate(bool increment);
};
