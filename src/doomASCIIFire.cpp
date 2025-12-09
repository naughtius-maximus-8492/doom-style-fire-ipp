#include "doomASCIIFire.h"

void doomASCIIFire::initRandomFunctions()
{
    // Init uniform distribution
    int randStateSize {};
    ippsRandUniformGetSize_16s(&randStateSize);
    uniformRandomState = reinterpret_cast<IppsRandUniState_16s *>(ippsMalloc_8u(randStateSize));
    ippsRandUniformInit_16s(uniformRandomState, defaultLowBoundUniform, this->decayRate, this->seededTime);

    // Init gaussian distribution
    int gaussianSize {};
    ippsRandGaussGetSize_16s(&gaussianSize);
    this->gaussianRandomState = reinterpret_cast<IppsRandGaussState_16s *>(ippsMalloc_16s(gaussianSize));
    ippsRandGaussInit_16s(this->gaussianRandomState, defaultMeanGauss, this->decayRate / 2, this->seededTime);
}

void doomASCIIFire::decayStep()
{
    // copy frames upwards
    tbb::parallel_for(static_cast<size_t>(0), static_cast<size_t>(this->intensityBufferHeight - 1), [&](const size_t i)
    {
        Ipp16s* row = &this->intensityBuffer[i * this->intensityBufferWidth];
        Ipp16s* rowBelow = &row[this->intensityBufferWidth];

        // Offset copy and rotate end or start values to simulate flickering
        int fireOffset = randDistribution(rng);

        for (int x = 0; x < this->intensityBufferWidth; x++)
        {
            this->setWeightedMean(&row[x], fireOffset);
        }

        // Ipp16s* offsetBuffer = ippsMalloc_16s(fireOffset);
        // const unsigned int positiveFireOffset = std::abs(fireOffset);
        // if (fireOffset == 0)
        // {
        //     ippsCopy_16s(rowBelow, row, this->frameBufferWidth);
        // }
        // if (fireOffset > 0)
        // {
        //     ippsCopy_16s(&rowBelow[this->frameBufferWidth - positiveFireOffset], offsetBuffer, positiveFireOffset);
        //     ippsCopy_16s(rowBelow, &row[positiveFireOffset], this->frameBufferWidth - positiveFireOffset);
        //     ippsCopy_16s(offsetBuffer, row, fireOffset);
        // }
        // else if (fireOffset < 0)
        // {
        //     ippsCopy_16s(rowBelow, offsetBuffer, positiveFireOffset);
        //     ippsCopy_16s(&rowBelow[positiveFireOffset], row, this->frameBufferWidth - positiveFireOffset);
        //     ippsCopy_16s(offsetBuffer, &row[this->frameBufferWidth - positiveFireOffset], positiveFireOffset);
        // }
        // ippsFree(offsetBuffer);


        // Generate random distribution
        Ipp16s* uniformBufferPos = &this->uniformRandomBuffer[i * intensityBufferWidth];
        ippsRandUniform_16s(uniformBufferPos, this->intensityBufferWidth, uniformRandomState);
        ippsSub_16s_I(uniformBufferPos, row, this->intensityBufferWidth);

        // Generate gaussian distribution
        Ipp16s* gaussBufferPos = &this->gaussRandomBuffer[i * intensityBufferWidth];
        ippsRandGauss_16s(gaussBufferPos, this->intensityBufferWidth, this->gaussianRandomState);
        ippsSub_16s_I(gaussBufferPos, row, this->intensityBufferWidth);

        ippsThreshold_LT_16s_I(row, this->intensityBufferWidth, minIntensity);

        ippsThreshold_GT_16s_I(row, this->intensityBufferWidth, maxIntensity);

    });


    // Update bottom row to have new flame sources
    int fireOffset = randDistribution(rng);

    Ipp16s* row = &this->intensityBuffer[this->intensityBufferSize - this->intensityBufferWidth];
    Ipp16s* rowBelow = &this->intensityBuffer[this->intensityBufferSize - this->intensityBufferWidth];

    Ipp16s* offsetBuffer = ippsMalloc_16s(fireOffset);
    const unsigned int positiveFireOffset = std::abs(fireOffset);
    if (fireOffset > 0)
    {
        ippsCopy_16s(&rowBelow[this->intensityBufferWidth - positiveFireOffset], offsetBuffer, positiveFireOffset);
        ippsCopy_16s(rowBelow, &row[positiveFireOffset], this->intensityBufferWidth - positiveFireOffset);
        ippsCopy_16s(offsetBuffer, row, fireOffset);
    }
    else if (fireOffset < 0)
    {
        ippsCopy_16s(rowBelow, offsetBuffer, positiveFireOffset);
        ippsCopy_16s(&rowBelow[positiveFireOffset], row, this->intensityBufferWidth - positiveFireOffset);
        ippsCopy_16s(offsetBuffer, &row[this->intensityBufferWidth - positiveFireOffset], positiveFireOffset);
    }
    ippsFree(offsetBuffer);
}

void doomASCIIFire::openConfig()
{
#ifdef WIN32
    clearScreen();
    std::cout << "ASCII Fire Configuration" << std::endl
            << "1) Set characters to use" << std::endl
            << "Q) Back to fire" << std::endl << std::endl
    << "Live Configuration Binds:" << std::endl
    << "UP/DOWN     : Fire Height" << std::endl
    << "LEFT/RIGHT  : Flicker Intensity" << std::endl
    << "A/D         : Fire Temperature" << std::endl
    << "W/S         : Frame Delay (FPS)" << std::endl
    << "F           : Background mode" << std::endl
    << "ESC         : Quit" << std::endl << std::endl
    << "Statistics:" << std::endl
    << "Frame buffer height : " << this->intensityBufferHeight << std::endl
    << "Frame buffer width  : " << this->intensityBufferWidth << std::endl
    << "Frame delay         : " << this->frameDelay << "ms (" << (1.0F / static_cast<float>(this->frameDelay)) * 1000 << " FPS)" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    bool exit = false;
    while (!exit)
    {
        if (detect_key_press('1'))
        {
            clearScreen();

            std::cout << "Current: \"" <<  this->characters << "\"" << std::endl
            << "Type characters to distribute as the temperature changes (low - high)" << std::endl
            << "> ";

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
            std::cin >> this->characters;

            this->openConfig();
        }
        else if (detect_key_press('Q'))
        {
            clearScreen();
            exit = true;
        }
    }
#endif
}

void doomASCIIFire::updateDecayRate(const bool increment)
{
    if (increment)
    {
        this->decayRate += 100;
    }
    else
    {
        this->decayRate -= 100;
    }

    if (this->decayRate < 2)
    {
        this->decayRate = 2;
    }

    ippsRandUniformInit_16s(uniformRandomState, defaultLowBoundUniform, decayRate, this->seededTime);
    ippsRandGaussInit_16s(this->gaussianRandomState, defaultMeanGauss, decayRate / 2 , this->seededTime);

}


void doomASCIIFire::updateFrame() const
{
    tbb::parallel_for(
        tbb::blocked_range<size_t>(0, this->intensityBufferSize, 1024),
        [&](const tbb::blocked_range<size_t>& range)
    {
            for (int i = range.begin(); i != range.end(); ++i)
            {
                const short intensity = this->intensityBuffer[i];
                Ipp8u* frameBufPos = &this->offsetCharFrameBuffer[i * fixedCharacterLength];

                bool newline = false;
                if (i % this->intensityBufferWidth == 0)
                {
                    newline = true;
                }

                this->setCharacter(intensity, frameBufPos, newline);
            }
    }
    );
}

void doomASCIIFire::setCharacter(const int intensity, Ipp8u* frameBufPos, const bool newline) const
{
    int position = 8;

    // Increment position to next point in char array we need to modify
    // Newlines discard a preceding 0 from part of the ANSII code and append a \n char on the end.
    // This ensures the size of the ANSII code stays consistent.
    if (newline)
    {
        position = 7;
    }

    this->setRGBValues(intensity, &frameBufPos[position]);

    // Increment position to next point in char array we need to modify
    position += 19;

    // Assign bg rgb value
    int backgroundIntensity {};

    if (this->backgroundMode)
    {
        backgroundIntensity = intensity;
    }
    else
    {
        backgroundIntensity = std::clamp(static_cast<int>(intensity * 0.1),
            static_cast<int>(minIntensity),
            static_cast<int>(maxIntensity * 0.025));
    }

    this->setRGBValues(backgroundIntensity, &frameBufPos[position]);

    // Increment position to next point in char array we need to modify
    position += 12;

    frameBufPos[position] = this->intensityToChar(intensity);
}

void doomASCIIFire::initConstantChars() const
{

    for (int i = 0; i < this->intensityBufferSize; ++i)
    {
        Ipp8u* frameBufPos = &this->offsetCharFrameBuffer[i * fixedCharacterLength];

        if (i % this->intensityBufferWidth == 0)
        {
            ippsCopy_8u(ansiiEscapeCodeNewline, frameBufPos, fixedCharacterLength);
        }
        else
        {
            ippsCopy_8u(ansiiEscapeCode, frameBufPos, fixedCharacterLength);
        }
    }


}

char doomASCIIFire::intensityToChar(const int intensity) const
{
    const int index = intensity * (this->characters.size() - 1) / maxIntensity;
    return this->characters[index];
}

void doomASCIIFire::setRGBValues(const int intensity, Ipp8u *frameBufPos) const
{
    constexpr int maxColourVal = 254;

    std::array rgb = {maxColourVal, maxColourVal, 0};

    // work out percentage to absolute zero
    const float percentage = static_cast<float>(intensity) / static_cast<float>(maxIntensity);

    const float colourBandOne = 0.8 * this->colourBandMultiplier;
    const float colourBandTwo = 0.2 * this->colourBandMultiplier;

    if (percentage >= colourBandOne) // white to yellow
    {
        rgb[2] = maxColourVal * this->normalise(percentage, colourBandOne, 1.0);
    }
    else if (percentage >= colourBandTwo) // yellow to red
    {
        rgb[2] = 0;
        rgb[1] = maxColourVal * this->normalise(percentage, colourBandTwo, colourBandOne) ;
    }
    else // red to black
    {
        rgb[2] = 0;
        rgb[1] = 0;
        rgb[0] = maxColourVal * this->normalise(percentage, 0.0, colourBandTwo);
    }

    int position = 0;
    for (const int colour : rgb)
    {
        frameBufPos[position++] = '0' + (colour / 100);
        frameBufPos[position++] = '0' + (colour / 10 % 10);
        frameBufPos[position++] = '0' + (colour % 10);

        if (position < 10)
        {
            frameBufPos[position++] = ';';
        }
    }

}


float doomASCIIFire::normalise(const float value, const float min, const float max)
{
    const float normalised = (value - min) / (max - min);
    return std::clamp(normalised, 0.0F, 1.0F);
}

void doomASCIIFire::setWeightedMean(Ipp16s *frameBufPos, int offset) const
{
    Ipp16s center = frameBufPos[this->intensityBufferWidth];
    Ipp16s left = frameBufPos[this->intensityBufferWidth - 1];
    Ipp16s right = frameBufPos[this->intensityBufferWidth + 1];

    frameBufPos[offset] = (center * 2 + left + right) / 4;
}

doomASCIIFire::doomASCIIFire(const int width, const int height)
    : decayRate { 65000 / height }
    , characters { defaultFlameGradient }
    , seededTime { time(nullptr) }
    , colourBandMultiplier { 1.0F }
    , backgroundMode(false)
    , frameDelay { defaultDelay }
    , flicker { 4 }
    , rng { std::random_device{}() }
    , randDistribution(defaultFlicker * -1, defaultFlicker)
{
    if (decayRate < 1)
    {
        decayRate = 10;
    }
    // Validate parameters
    if (width <= 0 || height <= 0)
    {
        throw std::invalid_argument("INVALID HEIGHT OR WIDTH: Must be greater than zero. Received arguments(" + std::to_string(height) + ", " + std::to_string(width) + ")");
    }

    // Calculate buffer sizes
    this->intensityBufferWidth = width;
    this->intensityBufferHeight = height;
    this->intensityBufferSize = width * height;

    // Allocate memory to buffers
    this->gaussRandomBuffer = ippsMalloc_16s(this->intensityBufferSize);
    this->uniformRandomBuffer = ippsMalloc_16s(this->intensityBufferSize);

    this->intensityBuffer = ippsMalloc_16s(this->intensityBufferSize);
    ippsSet_16s(minIntensity, this->intensityBuffer, this->intensityBufferSize - this->intensityBufferWidth);
    ippsSet_16s(maxIntensity / 1.2, &this->intensityBuffer[this->intensityBufferSize - this->intensityBufferWidth], this->intensityBufferWidth);
    ippsSet_16s(maxIntensity, &this->intensityBuffer[this->intensityBufferSize - this->intensityBufferWidth / 2], this->intensityBufferWidth / 8);

    this->charFrameBufferSize = this->intensityBufferSize * fixedCharacterLength + this->intensityBufferHeight + 4;
    this->startCharFrameBuffer = ippsMalloc_8u(this->charFrameBufferSize);
    this->startCharFrameBuffer[0] = '\033';
    this->startCharFrameBuffer[1] = '[';
    this->startCharFrameBuffer[2] = 'H';
    this->startCharFrameBuffer[this->charFrameBufferSize] = '\0';
    this->offsetCharFrameBuffer = &this->startCharFrameBuffer[3];

    this->initConstantChars();
    this->initRandomFunctions();

    ippsRandUniform_16s(this->uniformRandomBuffer, this->intensityBufferWidth, this->uniformRandomState);

    for (int i = 0; i < this->intensityBufferHeight; i++)
    {
        this->decayStep();
    }
}

doomASCIIFire::~doomASCIIFire()
{
    ippsFree(this->gaussRandomBuffer);
    ippsFree(this->uniformRandomBuffer);
    ippsFree(this->uniformRandomState);
    ippsFree(this->gaussianRandomState);

    // delete[] this->startCharFrameBuffer;
    ippsFree(this->startCharFrameBuffer);
    ippsFree(this->intensityBuffer);
}
