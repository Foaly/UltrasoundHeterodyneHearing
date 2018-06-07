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

#include "WavFileWriter.hpp"

#include <SPI.h>

namespace {

    // Teensy Audio Shield Defaults
    const uint8_t SDcard_CS_Pin    = 10;
    const uint8_t SDcard_MOSI_Pin  =  7;
    const uint8_t SDcard_SCK_Pin   = 14;

    // Use these with the Teensy 3.5 & 3.6 SD card (Should work, but not yet tested)
    //const uint8_t SDcard_CS_Pin    = BUILTIN_SDCARD
    //const uint8_t SDcard_MOSI_Pin  = 11  // not actually used
    //const uint8_t SDcard_SCK_Pin   = 13  // not actually used

    // Use these for the SD+Wiz820 or other adaptors (Should work, but not yet tested)
    //const uint8_t SDcard_CS_Pin    =  4
    //const uint8_t SDcard_MOSI_Pin  = 11
    //const uint8_t SDcard_SCK_Pin   = 13

    // The following functions takes integers in host byte order
    // and writes them to a stream as little endian

    void encode(File& file, uint16_t value) {
        uint8_t bytes[] =
        {
            static_cast<uint8_t>(value & 0xFF),
            static_cast<uint8_t>(value >> 8)
        };
        file.write(reinterpret_cast<const uint8_t*>(bytes), sizeof(bytes));
    }

    void encode(File& file, uint32_t value) {
        uint8_t bytes[] =
        {
            static_cast<uint8_t> (value & 0x000000FF),
            static_cast<uint8_t>((value & 0x0000FF00) >>  8),
            static_cast<uint8_t>((value & 0x00FF0000) >> 16),
            static_cast<uint8_t>((value & 0xFF000000) >> 24),
        };
        file.write(reinterpret_cast<const uint8_t*>(bytes), sizeof(bytes));
    }
}


WavFileWriter::WavFileWriter(AudioRecordQueue& queue) :
    m_isWriting(false),
    m_queue(queue)
{

}


bool WavFileWriter::open(const char *fileName, unsigned int sampleRate, unsigned int channelCount) {
    if (m_isWriting) {
        Serial.println("Cannot write WAV file. Already writing one.");
        return false;
    }


    // Initialize the SD card
    SPI.setMOSI(SDcard_MOSI_Pin);
    SPI.setSCK(SDcard_SCK_Pin);
    if (!(SD.begin(SDcard_CS_Pin))) {
        Serial.println("Unable to access the SD card while trying to write WAV file.");
        return false;
    }

    if (SD.exists(fileName)) {
        SD.remove(fileName);
    }

    m_file = SD.open(fileName, FILE_WRITE);
    if (!m_file) {
        Serial.println("Could not open file while trying to write WAV file.");
        return false;
    }

    m_queue.begin();
    m_isWriting = true;
    m_totalBytesWritten = 0;

    writeHeader(sampleRate, channelCount);
    return true;
}


bool WavFileWriter::isWriting() {
    return m_isWriting;
}


void WavFileWriter::writeHeader(unsigned int sampleRate, unsigned int channelCount) {
    // Write the main chunk ID
    uint8_t mainChunkId[4] = {'R', 'I', 'F', 'F'};
    m_file.write(mainChunkId, sizeof(mainChunkId));

    // Write the main chunk header
    uint32_t mainChunkSize = 0; // placeholder, will be written on closing
    encode(m_file, mainChunkSize);
    uint8_t mainChunkFormat[4] = {'W', 'A', 'V', 'E'};
    m_file.write(mainChunkFormat, sizeof(mainChunkFormat));

    // Write the sub-chunk 1 ("format") id and size
    uint8_t fmtChunkId[4] = {'f', 'm', 't', ' '};
    m_file.write(fmtChunkId, sizeof(fmtChunkId));
    uint32_t fmtChunkSize = 16;
    encode(m_file, fmtChunkSize);

    // Write the format (PCM)
    uint16_t format = 1;
    encode(m_file, format);

    // Write the sound attributes
    encode(m_file, static_cast<uint16_t>(channelCount));
    encode(m_file, static_cast<uint32_t>(sampleRate));
    uint32_t byteRate = sampleRate * channelCount * 2;
    encode(m_file, byteRate);
    uint16_t blockAlign = channelCount * 2;
    encode(m_file, blockAlign);
    uint16_t bitsPerSample = 16;
    encode(m_file, bitsPerSample);

    // Write the sub-chunk 2 ("data") id and size
    uint8_t dataChunkId[4] = {'d', 'a', 't', 'a'};
    m_file.write(dataChunkId, sizeof(dataChunkId));
    uint32_t dataChunkSize = 0; // placeholder, will be written on closing
    encode(m_file, dataChunkSize);

    m_totalBytesWritten += 44;
}


bool WavFileWriter::update() {
    if (!m_isWriting)
        return false;

    if (m_queue.available() < 2)
        return false;

    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(m_buffer, m_queue.readBuffer(), 256);
    m_queue.freeBuffer();
    memcpy(m_buffer + 256, m_queue.readBuffer(), 256);
    m_queue.freeBuffer();

    // write all 512 bytes to the SD card
    //elapsedMicros usec = 0;
    m_file.write(m_buffer, 512);
    // Uncomment these lines to see how long SD writes
    // are taking. A pair of audio blocks arrives every
    // 5802 microseconds, so hopefully most of the writes
    // take well under 5802 us. Some will take more, as
    // the SD library also must write to the FAT tables
    // and the SD card controller manages media erase and
    // wear leveling. The m_queue object can buffer
    // approximately 301700 us of audio, to allow time
    // for occasional high SD card latency, as long as
    // the average write time is under 5802 us.
    //Serial.print("SD write, us=");
    //Serial.println(usec);

    m_totalBytesWritten += 512;

    return true;
}


bool WavFileWriter::close() {
    if (!m_isWriting)
        return false;

    m_queue.end();
    while (m_queue.available() > 0) {
        m_file.write(reinterpret_cast<const uint8_t*>(m_queue.readBuffer()), 256);
        m_queue.freeBuffer();
        m_totalBytesWritten += 256;
    }

    Serial.print("Done! Max no. of audio blocks used: ");
    Serial.println(AudioMemoryUsageMax());
    Serial.print("Bytes written: ");
    Serial.println(m_totalBytesWritten);

    m_file.flush();

    // Update the main chunk size and data sub-chunk size
    uint32_t mainChunkSize = m_totalBytesWritten - 8;  // 8 bytes RIFF header
    uint32_t dataChunkSize = m_totalBytesWritten - 44; // 44 bytes RIFF + WAVE headers
    m_file.seek(4);
    encode(m_file, mainChunkSize);
    m_file.seek(40);
    encode(m_file, dataChunkSize);

    m_file.close();

    m_isWriting = false;
    return true;
}
