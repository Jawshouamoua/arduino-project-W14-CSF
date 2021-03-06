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
#include <tables/saw512_int8.h>
#include <Smooth.h>
#include <AutoMap.h> // maps unpredictable inputs to a range
#include <math.h>
#include <LowPassFilter.h>
 

// desired cutoff freq max and min for AutoMap



// desired intensity max and min, for AutoMap, note they're inverted for reverse dynamics
const int MIN_INTENSITY = 75;
const int MAX_INTENSITY = 5;

// desired mod speed max and min, for AutoMap, note they're inverted for reverse dynamics
const int MIN_MOD_SPEED = 10000;
const int MAX_MOD_SPEED = 1;

AutoMap kMapCarrierFreq(0,1023,0,60);
AutoMap kMapIntensity(0,1023,MIN_INTENSITY,MAX_INTENSITY);
AutoMap kMapModSpeed(0,5700,MIN_MOD_SPEED,MAX_MOD_SPEED);
AutoMap kMapCutoffFreq(0, 1023, 0, 255) ;

const int KNOB_PIN = 0; // set the input for the freq knob to analog pin 0
const int LDR1_PIN= 1; // set the analog input for fm_intensity to pin 1
const int LDR2_PIN= 2; // set the analog input for mod rate to pin 2
const int AMP_PIN = 3 ; //set analog input pin for amplitude
const int DUB_PIN = 4 ; //set pin for wubz

int vib_intensity = 0 ;

LowPassFilter lpf ;

Oscil<SAW512_NUM_CELLS, AUDIO_RATE> aCarrier(SAW512_DATA);
//Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA) ;
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kIntensityMod(COS2048_DATA);
//Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aVibrato(COS2048_DATA) ;

int mod_ratio = 5; // brightness (harmonics)
long fm_intensity; // carries control info from updateControl to updateAudio
int volume ;
unsigned long duration ;

// smoothing for intensity to remove clicks on transitions
float smoothness = 0.95f;
Smooth <long> aSmoothIntensity(smoothness);


void setup(){
  Serial.begin(115200); // set up the Serial output so we can look at the light level
  //lpf.setResonance(40) ;
  //aVibrato.setFreq(15.f) ;
  
  startMozzi(); // :))
}


void updateControl(){
  
  // read the freq knob
  int knob_value = mozziAnalogRead(KNOB_PIN); // value is 0-1023
  //Serial.println(knob_value) ;
  
  // map the knob to carrier frequency
  float carrier_freq = 13.75 * pow(2.0,(float)kMapCarrierFreq(knob_value)/12);
  
  //calculate the modulation frequency to stay in ratio
  int mod_freq = carrier_freq * mod_ratio;
  
  // set the FM oscillator frequencies
  aCarrier.setFreq_Q16n16(carrier_freq); 
  aModulator.setFreq(mod_freq);
  
  // read the light dependent resistor on the width Analog input pin
  int LDR1_value= mozziAnalogRead(LDR1_PIN); // value is 0-1023


  int LDR1_calibrated = kMapIntensity(LDR1_value);

  
  //push lfo forward
  long lfo = kIntensityMod.next()+128 ;
  
  
 // calculate the fm_intensity
  fm_intensity = ((long)LDR1_calibrated * lfo)>>8; // shift back to range after 8 bit multiply

  
  // read the light dependent resistor on the speed Analog input pin
  int wubWub = mozziAnalogRead(DUB_PIN); // value is 0-1023

  
  
  
  //use a float here for low frequencies
  float mod_speed = (float)kMapModSpeed(wubWub)/1000;
  
  kIntensityMod.setFreq(mod_speed);
  
  //read LDR pin 2 for vibrato intensity
  //vib_intensity = kMapCutoffFreq(mozziAnalogRead(LDR2_PIN)) ;
  
  
  //set low pass cutoff the same as mod_speed
  //lpf.setCutoffFreq(cutoff) ;

  
  //set volume
  volume = map(mozziAnalogRead(AMP_PIN), 0, 1023, 0, 255) ;
  
  Serial.println(); // finally, print a carraige return for the next line of debugging info
}


int updateAudio(){
  //long vibrato = vib_intensity * aVibrato.next() ;
   
  long modulation = aSmoothIntensity.next(fm_intensity) * aModulator.next();

  return (aCarrier.phMod(modulation) * volume) >> 8 ;
}


void loop(){
  audioHook();
}





