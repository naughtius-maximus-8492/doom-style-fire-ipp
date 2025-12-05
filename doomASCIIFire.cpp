#include "doomASCIIFire.h"

#include <iostream>
#include <ostream>


void doomASCIIFire::decayStep()
{
    // copy frames upwards
    for (int i = 0; i < frameBufferHeight - 1; ++i)
    {
        Ipp16s* row = frameBuffer + (i * frameBufferWidth);
        Ipp16s* rowBelow = frameBuffer + (i * frameBufferWidth) + frameBufferWidth;
        ippsCopy_16s(rowBelow, row, frameBufferWidth);

        // Generate random distribution to subtract from copied row
        Ipp16s* randomRow = ippsMalloc_16s(this->frameBufferWidth);


        ippsRandUniform_16s(randomRow, this->frameBufferWidth, randState);

        ippsSub_16s_I(randomRow, row, this->frameBufferWidth);
        ippsThreshold_LT_16s_I(row, this->frameBufferWidth, 0);

    }
}

void doomASCIIFire::printFrame()
{
    std::string frame = "\033[" + std::to_string(this->frameBufferWidth) + "D"   // left N
                        + "\033[" + std::to_string(this->frameBufferSize) + "A";  // up N
    int pos = 0;
    for (int i = 0; i < frameBufferHeight; ++i)
    {
        for (int j = 0; j < this->frameBufferWidth; ++j)
        {
            std::string intensity = std::to_string(this->frameBuffer[pos++]);

            std::string character = "0";

            if (this->frameBuffer[pos] == 0)
            {
                character = " ";
            }
            else if (this->frameBuffer[pos] < 25)
            {
                character = ".";
            }
            else if (this->frameBuffer[pos] < 150)
            {
                character = "o";
            }
            else if (this->frameBuffer[pos] < 200)
            {
                character = "O";
            }

            std::string colouredCharacter = "\033[38;5;" + intensity + "m" + character + "\033[0m";
            frame += colouredCharacter;
        }

        if (i < frameBufferHeight - 1)
        {
            frame+= '\n';
        }
    }

    std::cout << frame;
}

doomASCIIFire::doomASCIIFire()
    : frameBufferWidth(256)
    , frameBufferHeight(96)
    , frameBufferSize(frameBufferWidth * frameBufferHeight)
    , frameBuffer { ippsMalloc_16s(this->frameBufferSize) }
{
    ippsSet_16s(0, this->frameBuffer, this->frameBufferSize);
    Ipp16s* lastRow = this->frameBuffer + (this->frameBufferSize - this->frameBufferWidth);
    ippsSet_16s(254, lastRow, this->frameBufferWidth);

    int randStateSize = 0;
    ippsRandUniformGetSize_16s(&randStateSize);
    randState = reinterpret_cast<IppsRandUniState_16s *>(ippsMalloc_8u(randStateSize));
    ippsRandUniformInit_16s(randState, 0, 9, 789132);
}
