#include "AudioStream.h"

class Downsampler : public AudioStream
{
public:
    Downsampler();
    void setDivider(unsigned int divider);
    virtual void update();

private:
    //void setupNewBuffer();

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
    m_divider(1),
    m_firstRun(true)
{
    //setupNewBuffer();
}


void Downsampler::setDivider(unsigned int divider)
{
    if (divider > 0)
        m_divider = divider;
}


/*void Downsampler::setupNewBuffer()
{
    m_output = allocate();
    if (m_output)
        Serial.println("Failed to allocate audio buffer for downsampling!");
    m_dst = m_output->data;
    m_samplesWritten = 0;
}*/


// TODO: make non power of 2 safe, overlap
// TODO: refactor + cleanup
void Downsampler::update()
{
  //Serial.println("Starting update");

    if (m_firstRun)
    {
        m_output = allocate();
        if (!m_output){
            Serial.println("Failed to allocate audio buffer for downsampling!");
            return;
        }
        m_dst = m_output->data;
        m_samplesWritten = 0;
        m_firstRun = false;
    }

    audio_block_t* inputBlock;
    inputBlock = receiveReadOnly();

    if (!inputBlock)
        return;

    int16_t*       src = inputBlock->data;
    const int16_t* end = src + AUDIO_BLOCK_SAMPLES;

    for (; src < end; src += m_divider)
    {
        //Serial.println("%d", &*m_dst);
        //Serial.println(*m_dst);
        //Serial.println("%d", &*src);
        //Serial.println(*src);

        *m_dst++ = *src;
        m_samplesWritten++;

        //Serial.println(*m_dst);


        if (m_samplesWritten == AUDIO_BLOCK_SAMPLES) // off by one?
        {
            transmit(m_output);
            release(m_output);

            m_output = allocate();
            if (!m_output)
                Serial.println("Failed to allocate audio buffer for downsampling!");
            m_dst = m_output->data;
            m_samplesWritten = 0;
        }
    }

    release(inputBlock);
}
