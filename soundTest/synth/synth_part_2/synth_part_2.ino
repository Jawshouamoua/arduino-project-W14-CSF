/* 
 Joshua Kendall
Arduino instrument

  The circuit:
     Audio output on digital pin 9 

     Potentiometer connected to analog pin 0.
       Center pin of the potentiometer goes to the analog pin.
       Side pins of the potentiometer go to +5V and ground
  
     Light dependent resistor (LDR) and 5.1k resistor on analog pin 1:
       LDR from analog pin to +5V
       5.1k resistor from analog pin to ground
     
     Light dependent resistor (LDR) and 5.1k resistor on analog pin 2:
       LDR from analog pin to +5V
       5.1k resistor from analog pin to ground
       
     Potentiometer connect to analog pin 3 for volume.
     
     Potentiometer connected to analog pin 4
  

  With help from Tim Barrass 2013.

*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator 
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <tables/saw256_int8.h>
#include <Smooth.h>
#include <AutoMap.h> // maps unpredictable inputs to a range

 
// desired carrier frequency max and min, for AutoMap
const int MIN_CARRIER_FREQ = 22;
const int MAX_CARRIER_FREQ = 440;

// desired intensity max and min, for AutoMap, note they're inverted for reverse dynamics
const int MIN_INTENSITY = 700;
const int MAX_INTENSITY = 10;

// desired mod speed max and min, for AutoMap, note they're inverted for reverse dynamics
const int MIN_MOD_SPEED = 10000;
const int MAX_MOD_SPEED = 1;

AutoMap kMapCarrierFreq(0,1023,MIN_CARRIER_FREQ,MAX_CARRIER_FREQ);
AutoMap kMapIntensity(0,1023,MIN_INTENSITY,MAX_INTENSITY);
AutoMap kMapModSpeed(0,1023,MIN_MOD_SPEED,MAX_MOD_SPEED);
AutoMap kMapVolume(0,1023,0,255) ;
AutoMap kMapCutoff(0,1023,20,0) ;

const int KNOB_PIN = 0; // set the input for the knob to analog pin 0
const int LDR1_PIN=1; // set the analog input for fm_intensity to pin 1
const int LDR2_PIN=2; // set the analog input for mod rate to pin 2
const int KNOB_VOL = 3; //set volume pin
const int KNOB_PIN2 = 4 ; //set modulation ratio pin

Oscil<SAW256_NUM_CELLS, AUDIO_RATE> aCarrier(SAW256_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kIntensityMod(COS2048_DATA);

int mod_ratio = 5; // brightness (harmonics)
long fm_intensity; // carries control info from updateControl to updateAudio

int volume ; 

// smoothing for intensity to remove clicks on transitions
float smoothness = 0.95f;
Smooth <long> aSmoothIntensity(smoothness);


void setup(){
  Serial.begin(115200); // set up the Serial output so we can look at the light level
  startMozzi(); // :))
}


void updateControl(){
  // read the knob
  int knob_value = mozziAnalogRead(KNOB_PIN); // value is 0-1023
  
  // map the knob to carrier frequency
  int carrier_freq = kMapCarrierFreq(knob_value);
  
  //calculate the modulation frequency to stay in ratio
  
  int knob2_value = mozziAnalogRead(KNOB_PIN2) ;
  
  int mod_freq = kMapCarrierFreq(knob2_value) * mod_ratio;
  
  // set the FM oscillator frequencies
  aCarrier.setFreq(carrier_freq); 
  aModulator.setFreq(mod_freq);
  
  // read the light dependent resistor on the width Analog input pin
  int LDR1_value= mozziAnalogRead(LDR1_PIN); // value is 0-1023
  // print the value to the Serial monitor for debugging


  int LDR1_calibrated = kMapIntensity(LDR1_value);

  
 // calculate the fm_intensity
  fm_intensity = ((long)LDR1_calibrated * (kIntensityMod.next()+128))>>8; // shift back to range after 8 bit multiply

  // read the light dependent resistor on the speed Analog input pin
  int LDR2_value= mozziAnalogRead(LDR2_PIN); // value is 0-1023

  
  // use a float here for low frequencies
  float mod_speed = (float)kMapModSpeed(LDR2_value)/1000;

  kIntensityMod.setFreq(mod_speed);
  
  int knob_2 = mozziAnalogRead(KNOB_VOL) ;
  volume = kMapVolume(knob_2) ;
  
}

int updateAudio(){
  long modulation = aSmoothIntensity.next(fm_intensity) * aModulator.next();
  return (aCarrier.phMod(modulation) * volume) >> 8 ;
  
}


void loop(){
  audioHook();
}





