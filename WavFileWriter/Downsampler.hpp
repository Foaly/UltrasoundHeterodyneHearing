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
