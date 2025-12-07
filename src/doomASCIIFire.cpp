#include "doomASCIIFire.h"

void doomASCIIFire::initRandomFunctions()
{
    // Init uniform distribution
    int randStateSize {};
    ippsRandUniformGetSize_16s(&randStateSize);
    uniformRandomState = reinterpret_cast<IppsRandUniState_16s *>(ippsMalloc_8u(randStateSize));
    ippsRandUniformInit_16s(uniformRandomState, defaultLowBoundUniform, defaultUpperBoundUniform, this->seededTime);

    // Init gaussian distribution
    int gaussianSize {};
    ippsRandGaussGetSize_16s(&gaussianSize);
    this->gaussianRandomState = reinterpret_cast<IppsRandGaussState_16s *>(ippsMalloc_16s(gaussianSize));
    ippsRandGaussInit_16s(this->gaussianRandomState, defaultMeanGauss, defaultStandardDeviationGauss, this->seededTime);
}

void doomASCIIFire::decayStep() const
{
    std::mt19937 rng(std::random_device{}());
    // copy frames upwards
    for (unsigned int i = 0; i < frameBufferHeight - 1; ++i)
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
            << "Q) Back to fire" << std::endl << std::endl
    << "Live Configuration Binds:" << std::endl
    << "K/J    : Fire Height" << std::endl
    << "H/L    : Fire Temperature" << std::endl
    << "F      : Characters On/Off" << std::endl
    << "ESC    : Exit" << std::endl;

#ifdef WIN32

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    bool exit = false;
    while (!exit)
    {
        if (detect_key_press('1'))
        {
            system("cls");
            std::cout << "Current: \"" <<  this->characters << "\"" << std::endl
            << "Type characters to distribute as the temperature changes (low - high)" << std::endl
            << "> ";

            while (std::cin.rdbuf()->in_avail() > 0) {
                std::cin.get();
            }

            std::cin >> this->characters;
            this->openConfig();
        }
        else if (detect_key_press('2'))
        {
            system("cls");
            std::cout << "CURRENT (ms): " << this->frameDelay << std::endl
            << "> ";
            std::cin >> this->frameDelay;
            this->openConfig();
        }
        else if (detect_key_press('Q'))
        {
            exit = true;
        }
        if (detect_key_press(VK_ESCAPE))
        {
            this->running = false;
        }
    }
#endif
}

void doomASCIIFire::updateDecayRate(short decayRate) const
{
    ippsRandUniformInit_16s(uniformRandomState, defaultLowBoundUniform, decayRate, this->seededTime);
    ippsRandGaussInit_16s(this->gaussianRandomState, defaultMeanGauss, decayRate / 3 , this->seededTime);
}


std::string doomASCIIFire::getFrame() const
{
    std::string frame = "\033[H";

    frame.reserve((this->frameBufferSize + frameBufferPadding * 2) * maxCharacterSize);

    for (int i = 0; i < frameBufferSize; ++i)
    {
        const short intensity = this->frameBufferStart[i];

        std::string colouredCharacter = this->getCharacter(intensity);

        frame.append(colouredCharacter);

        if (i % frameBufferWidth == 0 && i > 0)
        {
            frame.append("\n");
        }
    }

    return frame;
}

std::string doomASCIIFire::getCharacter(const int intensity) const
{
    const char character = intensityToChar(intensity);

    float backgroundMultiplier = 0.1;

    if (backgroundMode)
    {
        backgroundMultiplier = 1.0;
    }

    const std::string rgbVal = intensityToColour(intensity);
    const std::string rgbValBackground = intensityToColour(intensity * backgroundMultiplier);

    std::string colouredCharacter = "\033[38;2;" + rgbVal + ";48;2;" + rgbValBackground + "m" + character + "\033[0m";

    return colouredCharacter;
}

char doomASCIIFire::intensityToChar(const int intensity) const
{
    const int index = intensity * (this->characters.size() - 1) / maxIntensity;
    return this->characters[index];
}

std::string doomASCIIFire::intensityToColour(const int intensity) const
{
    constexpr unsigned short maxColourVal = 254;
    int red = maxColourVal;
    int green = maxColourVal;
    int blue {};

    // work out percentage to absolute zero
    const float percentage = static_cast<float>(intensity) / static_cast<float>(maxIntensity);

    const float colourBandOne = 0.8 * colour_band_multiplier;
    const float colourBandTwo = 0.2 * colour_band_multiplier;

    if (percentage >= colourBandOne) // white to yellow
    {
        blue = maxColourVal * this->normalise(percentage, colourBandOne, 1.0);
    }
    else if (percentage >= colourBandTwo) // yellow to red
    {
        blue = 0;
        green = maxColourVal * this->normalise(percentage, colourBandTwo, colourBandOne) ;
    }
    else // red to black
    {
        blue = 0;
        green = 0;
        red = maxColourVal * this->normalise(percentage, 0.0, colourBandTwo);
    }
    return std::to_string(red) + ";" + std::to_string(green) + ";" + std::to_string(blue);
}

float doomASCIIFire::normalise(const float value, const float min, const float max)
{
    const float normalised = (value - min) / (max - min);
    return std::clamp(normalised, 0.0F, 1.0F);
}

void doomASCIIFire::calculateBufferSizes(int width, int height)
{
    this->frameBufferWidth = width;
    this->frameBufferHeight = height;
    this->frameBufferSize = width * height;
    this->frameBufferPadding = height * flicker;
    this->frameBufferFullSize = this->frameBufferSize + (this->frameBufferPadding * 2);
    this->frameBufferTopSize = this->frameBufferSize - width;
}

void doomASCIIFire::allocBuffers()
{
    frameBuffer = ippsMalloc_16s(frameBufferFullSize);
    uniformRandomBuffer = ippsMalloc_16s(this->frameBufferSize);
    gaussianRandomBuffer = ippsMalloc_16s(this->frameBufferSize);

    // I do not know why but getFrame() fails if the below line isn't here specifically
    this->frameBufferStart = &this->frameBuffer[this->frameBufferPadding];

    // Set default values
    ippsSet_16s(0, this->frameBufferStart, this->frameBufferFullSize);
    Ipp16s* lastRow = &this->frameBufferStart[this->frameBufferTopSize];
    ippsSet_16s(maxIntensity, lastRow, this->frameBufferWidth);
}

doomASCIIFire::doomASCIIFire(const int width, const int height)
    : characters { " .:*o|O0%&@#" }
    , seededTime { time(nullptr) }
    , running(true)
    , colour_band_multiplier { 1.0F }
    , backgroundMode(false)
    , frameDelay { defaultDelay }
{
    this->calculateBufferSizes(width, height);
    this->allocBuffers();
    this->initRandomFunctions();

}

doomASCIIFire::~doomASCIIFire()
{
    ippsFree(this->frameBuffer);
    ippsFree(this->uniformRandomBuffer);
    ippsFree(this->uniformRandomState);
    ippsFree(this->gaussianRandomBuffer);
    ippsFree(this->gaussianRandomState);
}
