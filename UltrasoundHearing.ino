#include "WavFileWriter/WavFileWriter.hpp"
#include "Utils.hpp"

#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>

const int Fs = 192000;

//const int myInput = AUDIO_INPUT_LINEIN;
const int myInput = AUDIO_INPUT_MIC;

AudioPlaySdWav           audioSD;
AudioSynthWaveform       sine;
AudioInputI2S            audioInput;
AudioOutputI2S           audioOutput;
AudioRecordQueue         queue1;

//record from mic
AudioConnection          patchCord1(audioInput, 0, queue1, 0);
AudioConnection          patchCord2(audioSD, 0, audioOutput, 0);
AudioConnection          patchCord3(audioSD, 0, audioOutput, 1);

AudioControlSGTL5000     audioShield;

elapsedMillis  elapsedMs;

WavFileWriter wavWriter(queue1);

void setup() {
    setI2SFreq(Fs);
    
    Serial.begin(9600);
    AudioMemory(60);
    audioShield.enable();
    audioShield.inputSelect(myInput);
    audioShield.micGain(40);  //0-63
    audioShield.volume(0.5);  //0-1
  
    sine.begin(WAVEFORM_SINE);
    sine.amplitude(0.9);
    sine.frequency(440);

    delay(1000);

    elapsedMs = 0;  
    Serial.println("Done initilizing! Starting now!");
}


void loop() {
    if (Serial.available() > 0) {
        // read the incoming byte:
        byte incomingByte = Serial.read();

        if ( incomingByte == '1' ) {
            Serial.println("Start recording!");
            wavWriter.open("Ultra.wav", Fs, 1);
        }
        if ( incomingByte == '2' ) {
            Serial.println("Stop recording!");
            wavWriter.close();
        }
    }

    if (wavWriter.isWriting())
        wavWriter.update();
}



