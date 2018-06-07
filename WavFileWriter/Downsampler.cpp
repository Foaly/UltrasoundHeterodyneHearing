////////////////////////////////////////////////////////////
/// Ultrasonic Hearing for the Teensy
/// Copyright (C) 2018  Maximilian Wagenbach (aka Foaly)
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.

/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.

/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <https://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////

#include "Downsampler.hpp"

Downsampler::Downsampler() :
    AudioStream(1, m_inputQueueArray),
    m_output(NULL),
    m_divider(1),
    m_remainder(0),
    m_firstRun(true)
{

}


void Downsampler::setDivider(uint8_t divider)
{
    if (divider > 0)
    {
        m_divider = divider;
        m_remainder = 0;
    }
}


void Downsampler::setupNewBuffer()
{
    m_output = allocate();
    if (!m_output)
        Serial.println("Failed to allocate audio buffer for downsampling!");
    m_dst = m_output->data;
    m_samplesWritten = 0;
}


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

    int16_t*       src  = inputBlock->data;
    const int16_t* end  = src + AUDIO_BLOCK_SAMPLES;
                   src += m_remainder;

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

    m_remainder = static_cast<uint8_t>(src - end);

    release(inputBlock);
}
