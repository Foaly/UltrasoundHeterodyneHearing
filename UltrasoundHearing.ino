#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2sIn;           //xy=115,157
AudioFilterStateVariable filter2;        //xy=283,184
AudioFilterStateVariable filter1;        //xy=488,178
AudioOutputI2S           i2sOut;           //xy=657,181
AudioConnection          patchCord1(i2sIn, 1, filter2, 0);
AudioConnection          patchCord2(filter2, 0, filter1, 0);
AudioConnection          patchCord3(filter1, 2, i2sOut, 0);
AudioConnection          patchCord4(filter1, 2, i2sOut, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=515,313
// GUItool: end automatically generated code



// Pin 13 has the LED on Teensy 3.0
int led = 13;
unsigned int count = 0;


// the setup routine runs once when you press reset:
void setup() 
{                
    Serial.begin(115200);

    AudioMemory(8);
    sgtl5000_1.enable();
    sgtl5000_1.volume(0.8);
    sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
    sgtl5000_1.micGain(60);

    // State variable (Chamberlin) filter, 12 db/octave
    filter2.frequency(8000);
    filter1.frequency(100);

    // somehow setting pin 13 to OUT breaks the mic input...
    //pinMode(led, OUTPUT);    
  
    delay(1000);
}

// the loop routine runs over and over again forever:
void loop() 
{
    /*digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);               // wait for a second
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    delay(500);               // wait for a second
    */
    
    Serial.print("Beep #");
    Serial.println(count);
    count = count + 1;
}
