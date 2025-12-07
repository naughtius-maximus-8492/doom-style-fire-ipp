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
    // copy frames upwards
    tbb::parallel_for(static_cast<size_t>(0), static_cast<size_t>(this->frameBufferHeight - 1), [&](const size_t i)
    {
        std::mt19937 rng(std::random_device{}());

        Ipp16s* row = this->frameBuffer[i];
        const Ipp16s* rowBelow = this->frameBuffer[i + 1];

        const int fireOffset = std::uniform_int_distribution<int>(flicker * -1,flicker)(rng);
        ippsCopy_16s(rowBelow, &row[fireOffset], frameBufferWidth - std::abs(fireOffset));

        // Generate random distribution
        Ipp16s* uniformBufferPos = &this->uniformRandomBuffer[i * frameBufferWidth];
        ippsRandUniform_16s(uniformBufferPos, this->frameBufferWidth, uniformRandomState);
        ippsSub_16s_I(uniformBufferPos, row, this->frameBufferWidth);

        // Generate gaussian distribution
        Ipp16s* gaussBufferPos = &this->gaussRandomBuffer[i * frameBufferWidth];
        ippsRandGauss_16s(gaussBufferPos, this->frameBufferWidth, this->gaussianRandomState);
        ippsSub_16s_I(gaussBufferPos, row, this->frameBufferWidth);

        ippsThreshold_LT_16s_I(row, this->frameBufferWidth, minIntensity);
        ippsThreshold_GT_16s_I(row, this->frameBufferWidth, maxIntensity);
    });

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

    frame.reserve(this->frameBufferSize * maxCharacterSize);

    for (int i = 0; i < this->frameBufferSize; ++i)
    // tbb::parallel_for(static_cast<size_t>(0), static_cast<size_t>(this->frameBufferHeight - 1), [&](const size_t i)
    {
        const short intensity = this->frameBuffer[i / this->frameBufferWidth][i % this->frameBufferWidth];

        std::string colouredCharacter = this->getCharacter(intensity);

        frame.append(colouredCharacter);

        if (i % this->frameBufferWidth == 0)
        {
            frame.append("\n");
        }
    }
    // );

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

doomASCIIFire::doomASCIIFire(const int width, const int height)
    : characters { " .:*o|O0%&@#" }
    , seededTime { time(nullptr) }
    , running(true)
    , colour_band_multiplier { 1.0F }
    , backgroundMode(false)
    , frameDelay { defaultDelay }
{
    // Calculate buffer sizes
    this->frameBufferWidth = width;
    this->frameBufferHeight = height;
    this->frameBufferSize = width * height;

    // Allocate memory to buffers
    this->gaussRandomBuffer = ippsMalloc_16s(this->frameBufferSize);
    this->uniformRandomBuffer = ippsMalloc_16s(this->frameBufferSize);

    this->frameBuffer = new Ipp16s*[this->frameBufferHeight];
    for (int i = 0; i < this->frameBufferHeight; i++)
    {
        this->frameBuffer[i] = ippsMalloc_16s(this->frameBufferWidth);

        if (i < this->frameBufferHeight - 1)
        {
            ippsSet_16s(0, this->frameBuffer[i], this->frameBufferWidth);
        }
        else
        {
            ippsSet_16s(maxIntensity, this->frameBuffer[i], this->frameBufferWidth);
        }
    }

    this->initRandomFunctions();
}

doomASCIIFire::~doomASCIIFire()
{
    ippsFree(this->gaussRandomBuffer);
    ippsFree(this->uniformRandomBuffer);
    ippsFree(this->uniformRandomState);
    ippsFree(this->gaussianRandomState);
}
