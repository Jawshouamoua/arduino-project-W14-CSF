

#include <Wire.h>
#include "nunchuck.h"

const unsigned int BAUD_RATE = 19200 ;

Nunchuck nunchuck ;

void setup() {
  Serial.begin(BAUD_RATE) ;
  nunchuck.initialize() ;
}

void loop() {
  if(nunchuck.update()) {
    Serial.print(nunchuck.joystick_x()) ;
    Serial.print(" ") ;
    Serial.print(nunchuck.joystick_y()) ;
    Serial.print(" ") ;
    Serial.print(nunchuck.x_acceleration()) ;
    Serial.print(" ") ;
    Serial.print(nunchuck.y_acceleration()) ;
    Serial.print(" ") ;
    Serial.print(nunchuck.z_acceleration()) ;
    Serial.print(" ") ;
    Serial.print(nunchuck.z_button()) ;
    Serial.print(" ") ;
    Serial.println(nunchuck.c_button()) ;
    
  }
}

  
