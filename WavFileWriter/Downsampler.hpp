
#ifndef DOWNSAMPLER_INCLUDE
#define DOWNSAMPLER_INCLUDE

#include "AudioStream.h"

class Downsampler : public AudioStream
{
public:
    Downsampler();
    void setDivider(uint8_t divider);
    virtual void update();

private:
    void setupNewBuffer();

    audio_block_t*    m_inputQueueArray[1];
    audio_block_t*    m_output;
    int16_t*          m_dst;
    uint32_t          m_samplesWritten;
    uint8_t           m_divider;
    uint8_t           m_remainder;
    bool              m_firstRun;
};

#endif // DOWNSAMPLER_INCLUDE
