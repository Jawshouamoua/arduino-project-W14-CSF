#include <avr/interrupt.h>

/******** sine wave parameters ******/
#define PI2 6.283185
#define AMP 127
#define OFFSET 128

/****** lookup table *******/
#define LENGTH 256
byte wave[LENGTH] ;

void setup() {
  for(int i = 0; i < LENGTH; i++) {
    float v = (AMP*sin((PI2/LENGTH)*i)) ;
    wave[i] = int(v+OFFSET) ;
  }
  
  /******* set timer1 for 8-bit fast PWM output *****/
  pinMode(9,OUTPUT) ;
  TCCR1B = (1 << CS10) ;
  TCCR1A |= (1 << COM1A1) ;
  TCCR1A |= (1 << WGM10) ;
  TCCR1B |= (1 << WGM12) ;
  
  /******* Set up timer2 to call ISR *******/
  TCCR2A = 0 ;
  TCCR2B = (1 << CS21) ;
  TIMSK2 = (1 << OCIE2A) ;
  OCR2A = 32 ;
  sei() ;
}

void loop() {
  
}

/******* Called every time TCNT2 = OCR2A *******/

ISR(TIMER2_COMPA_vect) {
  static byte index = 0 ;
  OCR1AL - wave[index++] ;
  asm("NOP;NOP") ;
  TCNT2 = 6 ;
}

