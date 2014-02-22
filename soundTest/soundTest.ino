int speakerPin = 9 ;

int sec = 60 ;
int sr = 44100 ;
double tones = 220 ;
const unsigned int PING_SENSOR_I_PIN = 6 ;
const unsigned int PING_SENSOR_O_PIN = 7 ;
const unsigned int BAUD_RATE = 9600 ;

void setup()
{
  Serial.begin(BAUD_RATE) ;

}

void loop(){

  
  pinMode(PING_SENSOR_O_PIN, OUTPUT) ;
  digitalWrite(PING_SENSOR_O_PIN, LOW) ;
  delayMicroseconds(2) ;
  
  digitalWrite(PING_SENSOR_O_PIN, HIGH) ;
  delayMicroseconds(5) ;
  digitalWrite(PING_SENSOR_O_PIN, LOW) ;
  
  pinMode(PING_SENSOR_I_PIN, INPUT) ;
  const unsigned long duration = pulseIn(PING_SENSOR_I_PIN, HIGH) ;
  if(duration == 0) {
    Serial.println("Warning, we didn't get a pulse") ;
  }
  else{
    
    Serial.println(duration) ;
  }
  delay(200) ;
   
  tone(speakerPin, duration) ; 
}
  
unsigned long microseconds_to_cm(const unsigned long microseconds) {
    return microseconds / 29 / 2 ;
    
}
  


