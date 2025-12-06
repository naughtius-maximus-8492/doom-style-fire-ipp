#include "doomASCIIFire.h"

void doomASCIIFire::decayStep()
{
    // copy frames upwards
    for (int i = 0; i < frameBufferHeight - 1; ++i)
    {
        Ipp16s* row = &frameBuffer[i * frameBufferWidth];
        Ipp16s* rowBelow = &frameBuffer[i * frameBufferWidth + frameBufferWidth];

        ippsCopy_16s(rowBelow, row, frameBufferWidth);

        // Generate random distribution
        ippsRandUniform_16s(this->randomRow, this->frameBufferWidth, randState);

        // subtract random distribution buffer from copied row to simulate fire decay
        ippsSub_16s_I(this->randomRow, row, this->frameBufferWidth);
        ippsThreshold_LT_16s_I(row, this->frameBufferWidth, 0);
    }
}

void doomASCIIFire::openConfig()
{
    system("cls");
    std::cout << "ASCII Fire Configuration" << std::endl
            << "1) Set characters to use" << std::endl
            << "2) Set colour temperature" << std::endl
            << "4) Set update delay" << std::endl
            << "5) Quit" << std::endl;

    bool exit = false;
    while (!exit)
    {
        if (GetAsyncKeyState('1') & 0x8000)
        {
            system("cls");
            std::cout << "Current: \"" <<  this->characters << "\"" << std::endl
            << "Type characters to distribute as the temperature changes (low - high)" << std::endl
            << "> ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard buffer
            std::cin >> this->characters;
            this->openConfig();
        }
        else if (GetAsyncKeyState('4') & 0x8000)
        {
            system("cls");
            std::cout << "CURRENT (ms): " << this->frameDelay << std::endl
            << "> ";
            std::string delay;
            std::cin >> delay;

            this->frameDelay = std::stoi(delay);
            this->openConfig();
        }
        else if (GetAsyncKeyState('5') & 0x8000)
        {
            exit = true;
        }
    }
}

void doomASCIIFire::wait() const
{
    std::this_thread::sleep_for(std::chrono::milliseconds(this->frameDelay));
}

void doomASCIIFire::updateDecayRate(int decayRate)
{
    ippsRandUniformInit_16s(randState, 0, decayRate, this->seededTime);
}

void doomASCIIFire::printFrame()
{
    std::string frame = "\033[" + std::to_string(this->frameBufferWidth) + "D"   // left N
                        + "\033[" + std::to_string(this->frameBufferSize) + "A";  // up N

    int pos = 0;

    for (int y = 0; y < frameBufferHeight; ++y)
    {
        for (int x = 0; x < this->frameBufferWidth; ++x)
        {
            const short intensity = this->frameBuffer[pos++];

            const char character = this->intensityToChar(intensity);

            const std::string colouredCharacter = "\033[38;2;" + this->intensityToColour(intensity) + "m" + character + "\033[0m";
            frame += colouredCharacter;
        }

        if (y < frameBufferHeight - 1)
        {
            frame+= '\n';
        }
    }

    std::cout << frame;
}

char doomASCIIFire::intensityToChar(const int intensity) const
{
    int index = intensity * (this->characters.size() - 1) / maxIntensity;
    return this->characters[index];
}

std::string doomASCIIFire::intensityToColour(const int intensity)
{
    int red = maxIntensity;
    int green = maxIntensity;
    int blue = maxIntensity;

    // work out percentage to absolute zero
    float percentage = static_cast<float>(intensity) / static_cast<float>(maxIntensity);

    float colourBandOne = 0.66 * colour_band_multiplier;
    float colourBandTwo = 0.33 * colour_band_multiplier;

    if (percentage >= colourBandOne)
    {
        blue = maxIntensity * this->normalise(percentage, colourBandOne, 1.0);
    }
    else if (percentage >= colourBandTwo)
    {
        blue = 0;
        green = maxIntensity * this->normalise(percentage, colourBandTwo, colourBandOne) ;
    }
    else
    {
        blue = 0;
        green = 0;
        red = maxIntensity * this->normalise(percentage, 0.0, colourBandTwo);
    }

    std::string colorCode = std::to_string(red) + ";" + std::to_string(green) + ";" + std::to_string(blue);
    return colorCode;

}

float doomASCIIFire::normalise(const float value, const float min, const float max)
{
    float interpolated = (value - min) / (max - min);

    return std::clamp(interpolated, 0.0F, 1.0F);
}

doomASCIIFire::doomASCIIFire(const int width, const int height)
    : frameBufferWidth(width)
    , frameBufferHeight(height)
    , frameBufferSize(frameBufferWidth * frameBufferHeight)
    , frameBuffer { ippsMalloc_16s(this->frameBufferSize) }
    , randomRow { ippsMalloc_16s(this->frameBufferWidth) }
    , frameDelay { 17 }
    , colour_band_multiplier { 1.0F }
    , characters { " .+*0#" }
    , seededTime { time(nullptr) }
{
    ippsSet_16s(0, this->frameBuffer, this->frameBufferSize);
    Ipp16s* lastRow = &this->frameBuffer[this->frameBufferSize - this->frameBufferWidth];
    ippsSet_16s(maxIntensity, lastRow, this->frameBufferWidth);

    int randStateSize = 0;
    ippsRandUniformGetSize_16s(&randStateSize);
    randState = reinterpret_cast<IppsRandUniState_16s *>(ippsMalloc_8u(randStateSize));
    ippsRandUniformInit_16s(randState, 0, 9, time(nullptr));
}

doomASCIIFire::~doomASCIIFire()
{
    ippsFree(this->frameBuffer);
    ippsFree(this->randomRow);
    ippsFree(this->randState);
}
