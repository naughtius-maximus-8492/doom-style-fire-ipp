#pragma once

#include <ipp.h>
#include <string>

class doomASCIIFire
{
private:
     int frameBufferWidth;
     int frameBufferHeight;
     int frameBufferSize;

     Ipp16s* frameBuffer;

     // Random related functions
     IppsRandUniState_16s* randState;


public:
     doomASCIIFire();

     void printFrame();
     void decayStep();
};