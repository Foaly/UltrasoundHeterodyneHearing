
#ifndef DOWNSAMPLER_INCLUDE
#define DOWNSAMPLER_INCLUDE

#include "AudioStream.h"

class Downsampler : public AudioStream
{
public:
    Downsampler();
    void setDivider(unsigned int divider);
    virtual void update();

private:
    void setupNewBuffer();

    audio_block_t*    m_inputQueueArray[1];
    audio_block_t*    m_output;
    int16_t*          m_dst;
    unsigned int      m_samplesWritten;
    unsigned int      m_divider;
    bool              m_firstRun;
};

#endif // DOWNSAMPLER_INCLUDE
