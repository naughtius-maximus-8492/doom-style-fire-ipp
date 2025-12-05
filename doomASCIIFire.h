#pragma once

#include <ipp.h>
#include <string>
#include <algorithm>
#include <limits>
#include <iostream>
#include <ostream>
#include <thread>
#define NOMINMAX
#include <Windows.h>

constexpr unsigned int maxIntensity = 254;

class doomASCIIFire
{
private:
     std::string characters;
     int frameBufferWidth;
     int frameBufferHeight;
     int frameBufferSize;
     int frameDelay;

     float colour_band_one;
     float colour_band_two;

     // Random related members
     Ipp16s* frameBuffer;
     Ipp16s* randomRow;
     IppsRandUniState_16s* randState;

     char intensityToChar(int intensity) const;
     std::string intensityToColour(int intensity);

     static float normalise(float value, float min, float max);

public:
     doomASCIIFire(int width, int height);
     ~doomASCIIFire();

     void printFrame();
     void decayStep();

     void openConfig();

     void wait() const;

};