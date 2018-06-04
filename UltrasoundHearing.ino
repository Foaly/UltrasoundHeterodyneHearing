#include "WavFileWriter/WavFileWriter.hpp"
#include "WavFileWriter/Downsampler.hpp"
#include "Utils.hpp"

#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>

const int samplingFs = 192000;
const int divider    = 8;
const int fileFs     = samplingFs / divider;

//const int micInput = AUDIO_INPUT_LINEIN;
const int micInput = AUDIO_INPUT_MIC;

AudioSynthWaveform       sine;
AudioInputI2S            audioInput;
AudioOutputI2S           audioOutput;
AudioRecordQueue         queue;
Downsampler              downsampler;

//record from mic
AudioConnection          patchCord1(audioInput, 0, downsampler, 0);
AudioConnection          patchCord2(downsampler, 0, queue, 0);
AudioConnection          patchCord3(downsampler, 0, audioOutput, 0);
AudioConnection          patchCord4(downsampler, 0, audioOutput, 1);

AudioControlSGTL5000     audioShield;

elapsedMillis  elapsedMs;

WavFileWriter wavWriter(queue);

const int triggerPin = 33;

void setup() {
    setI2SFreq(samplingFs);
    
    Serial.begin(9600);
    AudioMemory(150);
    audioShield.enable();
    audioShield.inputSelect(micInput);
    audioShield.micGain(50);  //0-63
    audioShield.volume(0.8);  //0-1
  
    sine.begin(WAVEFORM_SINE);
    sine.amplitude(0.9);
    sine.frequency(440);

    downsampler.setDivider(divider);

    pinMode(triggerPin, OUTPUT);
    digitalWrite(triggerPin, LOW);

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
            wavWriter.open("Ultra.wav", fileFs, 1);
        }
        if ( incomingByte == '2' ) {
            Serial.println("Stop recording!");
            wavWriter.close();
        }
    }

    // Hold the trigger pin high for at least 10 us
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    if (wavWriter.isWriting())
        wavWriter.update();
}



