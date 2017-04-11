SoftwareSerial transmitter(0, 1);

String message;

void setup() {
  Serial.begin(57600);
  transmitter.begin(8000);
  message = "SOS SOS SOS SOS SOS\n";
}

void sendSOS(){
  message.write(message);
}

void loop(){
  sendSOS();
}
