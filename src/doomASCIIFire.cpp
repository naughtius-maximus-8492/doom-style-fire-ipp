#include "doomASCIIFire.h"

void doomASCIIFire::initRandomFunctions()
{
    // Init uniform distribution
    int randStateSize = 0;
    ippsRandUniformGetSize_16s(&randStateSize);
    uniformRandomState = reinterpret_cast<IppsRandUniState_16s *>(ippsMalloc_8u(randStateSize));
    ippsRandUniformInit_16s(uniformRandomState, fixedLowBoundUniform, 9, this->seededTime);

    // Init gaussian distribution
    int gaussianSize{};
    ippsRandGaussGetSize_16s(&gaussianSize);
    this->gaussianRandomState = reinterpret_cast<IppsRandGaussState_16s *>(ippsMalloc_16s(gaussianSize));
    ippsRandGaussInit_16s(this->gaussianRandomState, fixedMeanGauss, 3, this->seededTime);
}

void doomASCIIFire::decayStep() const
{
    std::mt19937 rng(std::random_device{}());
    // copy frames upwards
    for (int i = 0; i < frameBufferHeight - 1; ++i)
    {
        Ipp16s* row = &frameBufferStart[i * frameBufferWidth];
        const Ipp16s* rowBelow = &frameBufferStart[i * frameBufferWidth + frameBufferWidth];

        int fireOffset = std::uniform_int_distribution<int>(flicker * -1,flicker)(rng);
        ippsCopy_16s(rowBelow, &row[fireOffset], frameBufferWidth);
    }
    // Generate random distribution
    ippsRandUniform_16s(this->uniformRandomBuffer, this->frameBufferTopSize, uniformRandomState);

    // Generate gaussian distribution
    ippsRandGauss_16s(this->gaussianRandomBuffer, this->frameBufferTopSize, this->gaussianRandomState);

    // subtract random distribution buffer from copied row to simulate fire decay
    ippsSub_16s_I(this->gaussianRandomBuffer, this->frameBufferStart, this->frameBufferTopSize);
    ippsSub_16s_I(this->uniformRandomBuffer, this->frameBufferStart, this->frameBufferTopSize);
    ippsThreshold_LT_16s_I(this->frameBufferStart, this->frameBufferSize, minIntensity);
    ippsThreshold_GT_16s_I(this->frameBufferStart, this->frameBufferSize, maxIntensity);
}

void doomASCIIFire::openConfig()
{
    system("cls");
    std::cout << "ASCII Fire Configuration" << std::endl
            << "1) Set characters to use" << std::endl
            << "2) Set update delay" << std::endl
            << "3) Back to fire" << std::endl << std::endl
    << "Live Configuration Binds:" << std::endl
    << "Up/Down    : Fire Height" << std::endl
    << "Left/Right : Fire Temperature" << std::endl
    << "F          : ASCII On/Off";

    bool exit = false;
    while (!exit)
    {
        if (GetAsyncKeyState('1') & 0x8000)
        {
            system("cls");
            std::cout << "Current: \"" <<  this->characters << "\"" << std::endl
            << "Type characters to distribute as the temperature changes (low - high)" << std::endl
            << "> ";
            std::cin >> this->characters;
            this->openConfig();
        }
        else if (GetAsyncKeyState('2') & 0x8000)
        {
            system("cls");
            std::cout << "CURRENT (ms): " << this->frameDelay << std::endl
            << "> ";
            std::cin >> this->frameDelay;
            this->openConfig();
        }
        else if (GetAsyncKeyState('3') & 0x8000)
        {
            exit = true;
        }
    }
}

void doomASCIIFire::updateDecayRate(int decayRate) const
{
    ippsRandUniformInit_16s(uniformRandomState, fixedLowBoundUniform, decayRate, this->seededTime);
    ippsRandGaussInit_16s(this->gaussianRandomState, fixedMeanGauss, decayRate / 2 , this->seededTime);
}

std::string doomASCIIFire::getFrame() const
{
    std::string frame;
    size_t stringSize = frameBufferSize * maxCharacterSize;
    frame.reserve(stringSize);
    frame.append("\033[" + std::to_string(this->frameBufferWidth) + "D"
                + "\033[" + std::to_string(this->frameBufferSize) + "A");

    for (int i = 0; i < frameBufferSize; ++i)
    {
        const short intensity = this->frameBufferStart[i];
        char character;
        std::string colouredCharacter;

        if (backgroundMode)
        {
            character = ' ';
            colouredCharacter = "\033[48;2;" + this->intensityToColour(intensity) + "m" + character + "\033[0m";
        }
        else
        {
            character = this->intensityToChar(intensity);
            colouredCharacter = "\033[38;2;" + this->intensityToColour(intensity) + ";48;2;" + this->intensityToColour(intensity*0.1) + "m" + character + "\033[0m";
        }
        frame.append(colouredCharacter);

        if (i % frameBufferWidth == 0)
        {
            frame.append("\n");
        }
    }

    frame.shrink_to_fit();
    return frame;
}

char doomASCIIFire::intensityToChar(const int intensity) const
{
    const int index = intensity * (this->characters.size() - 1) / maxIntensity;
    return this->characters[index];
}

std::string doomASCIIFire::intensityToColour(const int intensity) const
{
    int red = maxIntensity;
    int green = maxIntensity;
    int blue {};

    // work out percentage to absolute zero
    const float percentage = static_cast<float>(intensity) / static_cast<float>(maxIntensity);

    const float colourBandOne = 0.80 * colour_band_multiplier;
    const float colourBandTwo = 0.40 * colour_band_multiplier;

    if (percentage >= colourBandOne) // white to yellow
    {
        blue = maxIntensity * this->normalise(percentage, colourBandOne, 1.0);
    }
    else if (percentage >= colourBandTwo) // yellow to red
    {
        blue = 0;
        green = maxIntensity * this->normalise(percentage, colourBandTwo, colourBandOne) ;
    }
    else // red to black
    {
        blue = 0;
        green = 0;
        red = maxIntensity * this->normalise(percentage, 0.0, colourBandTwo);
    }

    return std::format("{:03}", red) + ";" + std::format("{:03}", green) + ";" + std::format("{:03}", blue);
}

float doomASCIIFire::normalise(const float value, const float min, const float max)
{
    const float normalised = (value - min) / (max - min);
    return std::clamp(normalised, 0.0F, 1.0F);
}

doomASCIIFire::doomASCIIFire(const int width, const int height)
    : characters { " .oO0#" }
    , frameBufferWidth(width)
    , frameBufferHeight(height)
    , frameBufferSize(width * height)
    , frameBufferTopSize(frameBufferSize - width)
    , frameBufferPadding(height * flicker)
    , frameDelay { 33 }
    , frameBuffer { ippsMalloc_16s(this->frameBufferSize + frameBufferPadding * 2) }
    , uniformRandomBuffer { ippsMalloc_16s(this->frameBufferSize) }
    , gaussianRandomBuffer { ippsMalloc_16s(this->frameBufferSize) }
    , seededTime { time(nullptr) }
    , colour_band_multiplier { 1.0F }
    , backgroundMode(false)
{
    this->frameBufferStart = &frameBuffer[frameBufferPadding];
    ippsSet_16s(0, this->frameBufferStart, this->frameBufferSize);
    Ipp16s* lastRow = &this->frameBufferStart[this->frameBufferTopSize];
    ippsSet_16s(maxIntensity, lastRow, this->frameBufferWidth);

    this->initRandomFunctions();
}

doomASCIIFire::~doomASCIIFire()
{
    //ippsFree(this->frameBuffer);
    ippsFree(this->uniformRandomBuffer);
    ippsFree(this->uniformRandomState);
    ippsFree(this->gaussianRandomBuffer);
    ippsFree(this->gaussianRandomState);
}
