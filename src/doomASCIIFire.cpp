#include "doomASCIIFire.h"

void doomASCIIFire::decayStep() const
{
    std::mt19937 rng(std::random_device{}());
    // copy frames upwards
    for (int i = 0; i < frameBufferHeight - 1; ++i)
    {
        Ipp16s* row = &frameBuffer[i * frameBufferWidth];
        const Ipp16s* rowBelow = &frameBuffer[i * frameBufferWidth + frameBufferWidth];

        const int fireOffset = std::uniform_int_distribution<int>(-2,2)(rng);
        ippsCopy_16s(rowBelow, &row[fireOffset], frameBufferWidth);
    }
    // Generate random distribution
    ippsRandUniform_16s(this->uniformRandomBuffer, this->frameBufferSize - frameBufferWidth, randState);

    // Generate gaussian distribution
    int gaussianSize{};
    ippsRandGaussGetSize_16s(&gaussianSize);
    IppsRandGaussState_16s* gaussianState = reinterpret_cast<IppsRandGaussState_16s *>(ippsMalloc_16s(gaussianSize));
    ippsRandGaussInit_16s(gaussianState,
        0,
        4,
        this->seededTime);
    ippsRandGauss_16s(this->gaussianRandomBuffer, this->frameBufferSize - frameBufferWidth, gaussianState);

    // subtract random distribution buffer from copied row to simulate fire decay
    ippsSub_16s_I(this->gaussianRandomBuffer, this->frameBuffer, this->frameBufferSize - frameBufferWidth);
    ippsSub_16s_I(this->uniformRandomBuffer, this->frameBuffer, this->frameBufferSize - frameBufferWidth);
    ippsThreshold_LT_16s_I(this->frameBuffer, this->frameBufferSize, minIntensity);
    ippsThreshold_GT_16s_I(this->frameBuffer, this->frameBufferSize, maxIntensity);
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
    ippsRandUniformInit_16s(randState, 0, decayRate, this->seededTime);
}

std::string doomASCIIFire::getFrame() const
{
    std::string frame;
    frame.reserve(frameBufferSize * 24);
    frame.append("\033[" + std::to_string(this->frameBufferWidth) + "D"
                + "\033[" + std::to_string(this->frameBufferSize) + "A");

    for (int i = 0; i < frameBufferSize; ++i)
    {
        const short intensity = this->frameBuffer[i];
        char character;
        std::string mode;

        if (backgroundMode)
        {
            mode = "48";
            character = ' ';
        }
        else
        {
            mode = "38";
            character = this->intensityToChar(intensity);
        }

        frame.append("\033[" + mode + ";2;" + this->intensityToColour(intensity) + ";48;2;" + this->intensityToColour(intensity*0.1) + "m" + character + "\033[0m");

        if (i % frameBufferWidth == 0)
        {
            frame.append("\n");
        }
    }

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
    const float colourBandTwo = 0.30 * colour_band_multiplier;

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
    const float normalised = (value - min) / (max - min);
    return std::clamp(normalised, 0.0F, 1.0F);
}

doomASCIIFire::doomASCIIFire(const int width, const int height)
    : characters { " .oO0#" }
    , frameBufferWidth(width)
    , frameBufferHeight(height)
    , frameBufferSize(frameBufferWidth * frameBufferHeight)
    , frameDelay { 33 }
    , frameBuffer { ippsMalloc_16s(this->frameBufferSize) }
    , uniformRandomBuffer { ippsMalloc_16s(this->frameBufferSize) }
    , gaussianRandomBuffer { ippsMalloc_16s(this->frameBufferSize) }
    , seededTime { time(nullptr) }
    , colour_band_multiplier { 1.0F }
    , backgroundMode(false)
{
    ippsSet_16s(0, this->frameBuffer, this->frameBufferSize);
    Ipp16s* lastRow = &this->frameBuffer[this->frameBufferSize - this->frameBufferWidth];
    ippsSet_16s(maxIntensity, lastRow, this->frameBufferWidth);

    int randStateSize = 0;
    ippsRandUniformGetSize_16s(&randStateSize);
    randState = reinterpret_cast<IppsRandUniState_16s *>(ippsMalloc_8u(randStateSize));
    ippsRandUniformInit_16s(randState, 0, 9, this->seededTime);
}

doomASCIIFire::~doomASCIIFire()
{
    //ippsFree(this->frameBuffer);
    ippsFree(this->uniformRandomBuffer);
    ippsFree(this->randState);
}
