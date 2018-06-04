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




// Downsampler.cpp
// TODO: the following code should be in its own seperate file, but Adruino is missing a proper project structure

Downsampler::Downsampler() :
    AudioStream(1, m_inputQueueArray),
    m_output(NULL),
    m_divider(1),
    m_firstRun(true)
{

}


void Downsampler::setDivider(unsigned int divider)
{
    if (divider > 0)
        m_divider = divider;
}


void Downsampler::setupNewBuffer()
{
    m_output = allocate();
    if (!m_output)
        Serial.println("Failed to allocate audio buffer for downsampling!");
    m_dst = m_output->data;
    m_samplesWritten = 0;
}


// TODO: make non power of 2 safe, overlap
void Downsampler::update()
{
    if (m_firstRun)
    {
        // I havn't found another way to allocate a buffer other than from here...
        setupNewBuffer();
        m_firstRun = false;
    }

    if(!m_output)
        Serial.println("Downsampler output buffer is NULL...");

    audio_block_t* inputBlock;
    inputBlock = receiveReadOnly();

    if (!inputBlock)
        return;

    int16_t*       src = inputBlock->data;
    const int16_t* end = src + AUDIO_BLOCK_SAMPLES;

    for (; src < end; src += m_divider)
    {
        *m_dst++ = *src;
        m_samplesWritten++;

        if (m_samplesWritten == AUDIO_BLOCK_SAMPLES)
        {
            transmit(m_output);
            release(m_output);

            setupNewBuffer();
        }
    }

    release(inputBlock);
}
