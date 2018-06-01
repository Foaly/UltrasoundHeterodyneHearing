////////////////////////////////////////////////////////////
//
// This code was inspired by the Recorder example of Paul Stoffregens Audio library
// and the SFML SoundFileWriterWav class
// Copyright (C) 2018 Maximilian Wagenbach (aka Foaly)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#include <SD.h>
#include <SD_t3.h>
#include <record_queue.h>

class WavFileWriter
{
public:
    WavFileWriter(AudioRecordQueue& queue);
    bool open(const char *fileName, unsigned int sampleRate, unsigned int channelCount);
    bool isWriting();
    bool update();
    bool close();

private:
    void writeHeader(unsigned int sampleRate, unsigned int channelCount);

    bool                  m_isWriting;
    File                  m_file;
    AudioRecordQueue&     m_queue;
    unsigned int          m_totalBytesWritten;
    byte                  m_buffer[512];
};

#include "WavFileWriter.cpp"