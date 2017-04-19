//UART baud rate: 9600 bps, (N-8-1)

 #include <SoftwareSerial.h>

SoftwareSerial gps(0, 1);

String location;

//Used to read in an entire line of GPS data
String data;

// Hold all of the message data
String message;

//Used to read in the available bytes from the receiver
//as they are available
char c;

void setup() {
  //pinMode(LED_BUILTIN, OUTPUT);       // Initialize LED pin
  
  //digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.begin(57600);
  
  while(!Serial){
    
  }

  Serial.println("Working");
    
  gps.begin(9600);
  
}

void parseData(){
    //Check if the line is GPGLL
    //This is actually wrong in other languuages
    if(data.substring(0, 5) == "GPGLL"){

      //Check if it is East
      if(data.indexOf('E') > 0){
        location = data.substring(7, data.indexOf('E'));

      //It is West
      }else{
        location = data.substring(7, data.indexOf('W')); 
      }

      //DEBUG
      Serial.println(location);
      
    }
}

// Method to send out SOS signal with current location
void sendSOS(){

  //Set baud to transmitter rate
  gps.begin(4800);
  
  message = "SOS Location is " + location + "\n";
  
  byte buffer[message.length()];

  int len = message.length();

  message.getBytes(buffer,len);

  for(int i = 0; i < len; ++i){
    gps.write(buffer[i]);
    //Serial.print((char)buffer[i]);
  }

  // Done transmitting, reset the baud to GPS rate
  gps.begin(9600);
}

void loop() {
  
  if(gps.available()){
    c = gps.read();
    
    //Check to see if there was a newline character
    if(c == '\n'){
      parseData();
      
      // For testing purposes
      sendSOS();
      
      data = "";
    }else{
      data += c;
    }

    //Serial.write(c);
    
    /*
    byte buffer[location.length()];
    location.getBytes(buffer, location.length());  
    
    for(int i = 0; i < ){
      Serial.write  
    }
    */
  }

  if(Serial.available()){
    Serial.println(Serial.read());
  }  
 }
