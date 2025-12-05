#pragma once

#include <ipp.h>
#include <string>
#include <algorithm>

constexpr unsigned int maxIntensity = 254;

class doomASCIIFire
{
private:
     std::string characters = " .-oO0#";
     int frameBufferWidth;
     int frameBufferHeight;
     int frameBufferSize;

     Ipp16s* frameBuffer;

     // Random related functions
     IppsRandUniState_16s* randState;

     char intensityToChar(int intensity);
     std::string intensityToColour(int intensity);
     float interpolate(float value, float min, float max);

public:
     doomASCIIFire();

     void printFrame();
     void decayStep();
};