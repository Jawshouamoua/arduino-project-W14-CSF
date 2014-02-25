#include <WProgram.h>
#include <Wire.h> 
#include "nunchuck.h"

#define NUNCHUCK_DEVICE_ID 0x52

void Nunchuck::initialize() {
	Wire.begin() ;
	Wire.beginTransmission(NUNCHUCK_DEVICE_ID) ;
	Wire.write(0x40) ;
	Wire.write(0x00) ;
	Wire.endTransmission() ;
	update() ;
}

bool Nunchuck::update() {
	delay(1) ;
	Wire.requestFrom(NUNCHUCK_DEVICE_ID, NUNCHUCK_BUFFER_SIZE) ;
	int byte_counter = 0 ;
	while(Wire.available() && byte_counter < NUNCHUCK_BUFFER_SIZE)
		_buffer[byte_counter++] = decode_byte(Wire.read()) ;
	request_data() ;
	return byte_counter == NUNCHUCK_BUFFER_SIZE ;
	
}

void Nunchuck::request_data() {
	Wire.beginTransmission(NUNCHUCK_DEVICE_ID) ;
	Wire.write(0x00) ;
	Wire.endTransmission() ;
}

char Nunchuck::decode_byte(const char b) {
	return (b ^ 0x17) + 0x017 ;
	
}