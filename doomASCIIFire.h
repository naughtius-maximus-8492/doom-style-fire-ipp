#pragma once

#include <ipp.h>
#include <string>
#include <algorithm>

constexpr unsigned int maxIntensity = 254;

class doomASCIIFire
{
private:
     int frameBufferWidth;
     int frameBufferHeight;
     int frameBufferSize;

     Ipp16s* frameBuffer;

     // Random related functions
     IppsRandUniState_16s* randState;

     std::string intensityToColour(int intensity);
     int interpolate(float value, float min, float max);

public:
     doomASCIIFire();

     void printFrame();
     void decayStep();
};