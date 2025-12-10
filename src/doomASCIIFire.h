#pragma once

#include <ipp.h>
#include <tbb/tbb.h>
#include <string>
#include <thread>
#include <iostream>
#include <random>
#include "utils.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "PerlinNoise.hpp"


constexpr int minIntensity = 0;
constexpr int maxIntensity = 16384;

constexpr int defaultLowBoundUniform = 0;
constexpr int defaultMeanGauss = -5;

constexpr unsigned int defaultDelay = 67;
constexpr unsigned int fixedCharacterLength = 44;

constexpr int defaultFlicker = 1;

constexpr const char *defaultFlameGradient = " .:;+*%##";

constexpr Ipp8u ansiiEscapeCodeNewline[] = "\033[38;2;000;000;000m\033[48;2;000;000;000m \033[0m\n";
constexpr Ipp8u ansiiEscapeCode[] = "\033[038;2;000;000;000m\033[48;2;000;000;000m \033[0m";

class doomASCIIFire
{
private:
     int decayRate;
     std::string characters;
     int intensityBufferWidth;
     int intensityBufferHeight;

     Ipp16s* charIntensityBuffer;
     Ipp16s* embersIntensityBuffer;

     Ipp16s* gaussRandomBuffer;
     Ipp16s* uniformRandomBuffer;
     IppsRandUniState_16s* uniformRandomState;
     IppsRandGaussState_16s* gaussianRandomState;

     void initRandomFunctions();

     char intensityToChar(int intensity) const;
     void setRGBValues(int intensity, Ipp8u* frameBufPos) const;
     void setCharacter(int intensity, Ipp8u* frameBufPos, bool newline) const;

     void initConstantChars() const;

     static float normalise(float value, float min, float max);
     void setWeightedMean(Ipp16s* frameBufPos, int offset) const;

     void decayStep(Ipp16s* row, bool useWeightedMean, int height);


     time_t seededTime;
     std::mt19937 rng;
     std::uniform_int_distribution<int> flickerRandomDistribution;
     std::uniform_int_distribution<int> embersRandomDistribution;

     // used for internal calculation that don't want to in
     Ipp8u* offsetCharFrameBuffer;

     // Perlin noise for base fire
     unsigned int perlinNoisePos;
     siv::PerlinNoise::seed_type perlinSeed;
     siv::PerlinNoise perlin;

public:
     // Used for getting the entire frame buffer
     Ipp8u* startCharFrameBuffer;
     int charFrameBufferSize;
     int intensityBufferSize;
     float colourBandMultiplier;
     bool backgroundMode;
     int frameDelay;
     int flicker;

     doomASCIIFire(int width, int height);
     ~doomASCIIFire();

     void updateFrame() const;

     void decayFrame();

     void openConfig(KeyHandler& handler);

     void updateDecayRate(bool increment);


};
