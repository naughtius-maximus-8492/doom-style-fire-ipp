#pragma once

#include <ipp.h>
#include <string>
#include <thread>
#include <iostream>
#define NOMINMAX
#include <Windows.h>
#include <random>

constexpr unsigned int maxIntensity = 254;

class doomASCIIFire
{
private:
     std::string characters;
     int frameBufferWidth;
     int frameBufferHeight;
     int frameBufferSize;
     int frameDelay;

     Ipp16s* frameBuffer;
     Ipp16s* randomRow;
     IppsRandUniState_16s* randState;

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

     void wait() const;

     void updateDecayRate(int decayRate) const;

     float colour_band_multiplier;
     bool backgroundMode;
};