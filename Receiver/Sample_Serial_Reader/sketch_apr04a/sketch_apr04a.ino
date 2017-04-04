#include <SoftwareSerial.h>

SoftwareSerial mySerial(0, 1);

void setup() {
  Serial.begin(57600);
  while(!Serial){
        
  }

  Serial.println("Goodnight moon!");

  mySerial.begin(9600);
  mySerial.println("Is it working?");
}

void loop() {
  if(mySerial.available()){
      Serial.write(mySerial.read());
  }

  if(Serial.available()){
    Serial.println(Serial.read());    
  }
}
