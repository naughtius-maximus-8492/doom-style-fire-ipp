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

        // Generate random distribution
        Ipp16s* randomRow = ippsMalloc_16s(this->frameBufferWidth);
        ippsRandUniform_16s(randomRow, this->frameBufferWidth, randState);

        // subtract random distribution buffer from copied row to simulate fire decay
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
            short intensity = this->frameBuffer[pos++];

            char character = this->intensityToChar(intensity);

            // std::string colouredCharacter = "\033[38;5;100m" + character + "\033[0m";
            std::string colouredCharacter = "\033[38;2;" + this->intensityToColour(intensity) + "m" + character + "\033[0m";
            frame += colouredCharacter;
        }

        if (i < frameBufferHeight - 1)
        {
            frame+= '\n';
        }
    }

    std::cout << frame;
}

char doomASCIIFire::intensityToChar(int intensity)
{
    int index = intensity * (this->characters.size() - 1) / maxIntensity;
    return this->characters[index];
}

std::string doomASCIIFire::intensityToColour(int intensity)
{
    int red = 254;
    int green = 254;
    int blue = 254;

    // work out percentage to absolute zero
    float percentage = static_cast<float>(intensity) / static_cast<float>(maxIntensity);

    if (percentage >= 0.66)
    {
        blue = maxIntensity * this->interpolate(percentage, 0.66, 1.0);
    }
    else if (percentage >= 0.33)
    {
        blue = 0;
        green = maxIntensity * this->interpolate(percentage, 0.33, 0.66) ;
    }
    else
    {
        blue = 0;
        green = 0;
        red = maxIntensity * this->interpolate(percentage, 0.0, 0.33);
    }

    std::string colorCode = std::to_string(red) + ";" + std::to_string(green) + ";" + std::to_string(blue);
    return colorCode;

}

float doomASCIIFire::interpolate(float value, float min, float max)
{
    float interpolated = (value - min) / (max - min);

    return std::clamp(interpolated, 0.0F, 1.0F);
}

doomASCIIFire::doomASCIIFire()
    : frameBufferWidth(256)
    , frameBufferHeight(96)
    , frameBufferSize(frameBufferWidth * frameBufferHeight)
    , frameBuffer { ippsMalloc_16s(this->frameBufferSize) }
{
    ippsSet_16s(0, this->frameBuffer, this->frameBufferSize);
    Ipp16s* lastRow = this->frameBuffer + (this->frameBufferSize - this->frameBufferWidth);
    ippsSet_16s(maxIntensity, lastRow, this->frameBufferWidth);

    int randStateSize = 0;
    ippsRandUniformGetSize_16s(&randStateSize);
    randState = reinterpret_cast<IppsRandUniState_16s *>(ippsMalloc_8u(randStateSize));
    ippsRandUniformInit_16s(randState, 0, 9, 789132);
}
