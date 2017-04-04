//UART baud rate: 9600 bps, (N-8-1)

 #include <SoftwareSerial.h>

SoftwareSerial gps(0, 1);

void setup() {
  Serial.begin(57600);
  while(!Serial){}
  
  pinMode(LED_BUILTIN, OUTPUT);       // Initialize LED pin
  gps.begin(9600);  
}
void loop() {
   if(mySerial.available()){
      Serial.write(mySerial.read());
  }

  if(Serial.available()){
    Serial.println(Serial.read());    
  }
 }
