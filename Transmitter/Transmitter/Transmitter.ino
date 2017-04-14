#include <SoftwareSerial.h>
 
SoftwareSerial transmitter(0, 1);

String message = "";

void setup() {
  Serial.begin(57600);
  transmitter.begin(4800);
  message = "SOS SOS SOS SOS SOS\n";
}

void sendSOS(){
  byte buffer[message.length()];

  int len = message.length();

  message.getBytes(buffer,len);

  for(int i = 0; i < len; ++i){
    transmitter.write((char)buffer[i]);
    Serial.print((char)buffer[i]);
  }
}

void loop(){
  sendSOS();
}
