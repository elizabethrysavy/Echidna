#include <CurieIMU.h>
#include <PulseSensorBPM.h>
#include <SoftwareSerial.h>

//set pin numbers:
const int power = 7;   //pin number for power switch
const int cancel = 8; //pin number for alarm cancel button
const int criticalLED = 2; //pin number for LED for critical warning
const int buzzer = 4; //pin number for buzzer
const int pulseSensor = A0; //pin number for pulse sensor
const int tempSensor = A1; //pin number for temperature sensor

const int wantPrint = HIGH; //will it be connected to the computer at this demo?
const int highTemp = 103; //high critical threshold for temperature
const int lowTemp = 93; //low critical threshold for temperature
const int highRate = 220; //high critical threshold for pulse
const int lowRate = 40; //low critical threshold for pulse

//global variables
unsigned int criticalCount; //counts number of times user has been in critical range
const int WAIT_TIME = 45; //seconds to push cancel button after sensors read as critical
int switchState; //used to read the on/off switch
int buttonState; //used to read the cancel button
const int x = 5; /*MIGHT NEED TO CHANGE THIS VALUE*/ //i don't know what this is.
//int cc; //do not use this anymore. Need an unsigned int.
unsigned int cc;
int prevTime;
unsigned long loopTime = 0;          // get the time since program started
unsigned long interruptsTime = 0;    // get the time when free fall event is detected
int prevSS; //previous switch state
int lastPrint;

//pulse sensor variables, used for internal workings of pulse sensor ONLY
 const unsigned long MICROS_PER_READ = 2 * 1000L;
 const boolean REPORT_JITTER_AND_HANG = HIGH;
 const long OFFSET_MICROS = 1L;
 unsigned long wantMicros;
 long minJitterMicros;
 long maxJitterMicros;
 unsigned long lastReportMicros;
 byte samplesUntilReport;
 const byte SAMPLES_PER_SERIAL_SAMPLE = 20;
 int BPM; //This will have the value of the Beats Per Minute of the person's heart rate.
 PulseSensorBPM pulseDetector(pulseSensor, MICROS_PER_READ / 1000L);

 //GPS variables
 SoftwareSerial gps(0,1); //gps is an item
 String location; //location of user
 String datas; //information received from GPS locator
 char c; 

void setup() {
  delay(1000); //wait a moment so avoid triggering free fall mode
  //initialize inputs and outputs
  pinMode(power, INPUT);
  pinMode(cancel, INPUT);
  pinMode(criticalLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  //initialize variables and states
  criticalCount = 0;
  digitalWrite(criticalLED, LOW);
  digitalWrite(buzzer, LOW);
  //cc = (30/(x * pow(10, -6))) * 0.9; //90% of the reads taken in 30s
  //cc = 5400000; //this is the actual value for 30 seconds
  cc = 100000; //testing purposes: Lowering time it takes to trigger value
  prevTime = micros();
  prevSS = digitalRead(power);
  lastPrint = millis();

  Serial.begin(9600); // initialize Serial communication
  while (!Serial) ;   // wait for serial port to connect.

  //Initialise the IMU 
  CurieIMU.begin();
  CurieIMU.attachInterrupt(eventCallback);

  //Enable Free Fall Detection 
  CurieIMU.setDetectionThreshold(CURIE_IMU_FREEFALL, 1000); // 1g=1000mg
  CurieIMU.setDetectionDuration(CURIE_IMU_FREEFALL, 50);  // 50ms
  CurieIMU.interrupts(CURIE_IMU_FREEFALL);

  //Initialize PulseSensor
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
  lastReportMicros = 0L;
  resetJitter();
  wantMicros = micros() + MICROS_PER_READ;

  //Initialize GPS
  gps.begin(9600);
  if(wantPrint == HIGH)
  {
    Serial.println("Starting The Echidna.");
    Serial.println();
  }
}

void loop() 
{
  switchState = digitalRead(power);
  if (switchState == LOW) { //if power switch is off
    if (wantPrint == HIGH)
    {
      Serial.println("The Echidna is off. Switch to on to begin operation.");
      Serial.println();
      delay(500); //wait a moment if printing
    }
    prevSS = switchState;
    return;
  }

  if (prevSS == LOW) { //if system was off
    //reset variables
    prevTime = micros();
    criticalCount = 0;
    digitalWrite(criticalLED, LOW);
    digitalWrite(buzzer, LOW);
    samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
    lastReportMicros = 0L;
    resetJitter();
    wantMicros = micros() + MICROS_PER_READ;
    lastPrint = millis();
    gps.begin(9600);
  }
  prevSS = switchState;
  //read monitors
  int heart = readHeartMonitor();
  int temp = readTempSensor(); 
  detectFall();
  readGPS();
  
  if (isCritical(heart, temp) == HIGH) 
  {
    criticalCount++;
  }
  
  else if (criticalCount > 0) 
  {
    criticalCount--;
  }
  
  if (criticalCount >= cc) {
    critical();
  }

  if (wantPrint == HIGH && millis() - lastPrint >= 2000){
    lastPrint = millis();
    printForDemo(heart, temp);
  }

  delayMicroseconds(x); //wait a short amount of time before reading sensors again

}

int readHeartMonitor() {
 unsigned long nowMicros = micros();
 if ((long) (wantMicros - nowMicros) > 1000L) {
   return BPM;
 }
 
 if ((long) (wantMicros - nowMicros) > 3L + OFFSET_MICROS) {
   delayMicroseconds((unsigned int) (wantMicros - nowMicros) - OFFSET_MICROS);
   nowMicros = micros();    
 }
 
 long jitterMicros = (long) (nowMicros - wantMicros);
 
 if (minJitterMicros > jitterMicros) {
   minJitterMicros = jitterMicros;
 }
 
 if (maxJitterMicros < jitterMicros) {
   maxJitterMicros = jitterMicros;
 }
 
 wantMicros = nowMicros + MICROS_PER_READ;
 boolean QS = pulseDetector.readSensor();
 
 if (--samplesUntilReport == (byte) 0) {
   samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
 }
 
 if (QS) {
   BPM = pulseDetector.getBPM();
 }
 return BPM;

}


int readTempSensor() { 
  int reading = analogRead(tempSensor); //reads the value on the pin

  //converts reading into a voltage
  float voltage = reading * 5.0; 
  voltage /= 1024.0;

  //converts reading to fahrenheit
  float temperatureF = (((voltage - 0.5) * 100) * 9.0 / 5.0) + 32.0;

  return (int) temperatureF;
}


bool isCritical(int heart, int temp) { 
  if (heart < lowRate or (heart > highRate and heart < 300))
  {
    return HIGH;
  }
  if (temp < lowTemp or temp > highTemp) { 
    return HIGH;
  }
  else return LOW;
}


void detectFall() { //read accelerometer to detect fall
  //detect freefall
  loopTime = millis();
  if (abs(loopTime - interruptsTime) < 1000 )
  {
    critical();
  }  
  else
    return;
}


static void eventCallback() {
  if (CurieIMU.getInterruptStatus(CURIE_IMU_FREEFALL)) {
    if(wantPrint == HIGH)
      Serial.println("free fall detected! ");
      Serial.println();
    interruptsTime = millis();
  }
}


void critical() {
  unsigned long startTime = millis();
  //turn on LED and buzzer
  digitalWrite(criticalLED, HIGH);
  digitalWrite(buzzer, HIGH);
  while (millis() - startTime < (WAIT_TIME * 1000)) 
  {
    buttonState = digitalRead(cancel);
    if (wantPrint == HIGH && millis() - lastPrint >= 1000)
    {
      Serial.println("CRITICAL: Push button if no help is required.\n");
      lastPrint = millis();
    }
    if (buttonState == LOW) { //if person indicates they are okay
      digitalWrite(criticalLED, LOW); //turn of LED and buzzer
      digitalWrite(buzzer, LOW);
      criticalCount = 0;
      if (wantPrint == HIGH)
      {
        Serial.println("User has indicated that no help is required.");
        Serial.println("Resuming regular operation.");
        Serial.println();
      }
      return;
    }
  }
  //if button hasnt been pressed, implement emergency procedure
  emergencyProcedure();
}


void emergencyProcedure() { //user read to be in critical condition
  digitalWrite(criticalLED, LOW); //turn off LED to conserve power
  digitalWrite(buzzer, HIGH); //make sure buzzer is on so person can be located easier
  gps.begin(4800);
  while(1){
    sendSOS();
    if (wantPrint == HIGH && millis() - lastPrint >= 1000){
      Serial.println("DANGER: Help is being contacted. Press button if no help is required.");
      Serial.println();
      lastPrint = millis();
    }
    buttonState = digitalRead(cancel);
    if (buttonState == LOW) { //if person indicates they are okay
      digitalWrite(buzzer, LOW);
      criticalCount = 0;
      if (wantPrint == HIGH)
      {
        Serial.println("User has indicated that no help is required.");
        Serial.println("Resuming regular operation.");
        Serial.println();
      }
      gps.begin(9600);
      return;
    }
  }
}

void resetJitter() 
{
 // min = a number so large that any value will be smaller than it;
 // max = a number so small that any value will be larger than it.
 minJitterMicros = 60 * 1000L;
 maxJitterMicros = -1;
}

void parseData(){
  //Check if the line is GPGLL
  if(datas.substring(0, 5) == "GPGLL"){
    //Check if it is East
    if(datas.indexOf('E') > 0){
      location = datas.substring(7, datas.indexOf('E'));
    }
    //It is West
    else{
      location = datas.substring(7, datas.indexOf('W')); 
    }
  }
}

void readGPS() { /*THIS MIGHT NEED INPUTS OR SOMETHING*/
   if(gps.available()){
    c = gps.read();
    //Check to see if there was a newline character
    if(c == '\n'){
      parseData();
      datas = "";
    }
    else{
      datas += c;
    }
    /*Serial.write(c);*/
    
    /*
    byte buffer[location.length()];
    location.getBytes(buffer, location.length());  
    
    for(int i = 0; i < ){
      Serial.write  
    }
    */
  }

  /*if(Serial.available()){
    Serial.println(Serial.read());
  }*/
}

void sendSOS()
{
  String message = "SOS Location is " + location;
  byte buffer[message.length()];
  int len = message.length();
  message.getBytes(buffer,len);
  for(int i = 0; i < len; ++i)
  {
    gps.write(buffer[i]);
  }
}

void printForDemo(int heart, int temp) {
  Serial.print("Heart Rate: ");
  Serial.println(heart);
  Serial.print("Temperature: ");
  Serial.println(temp);
  Serial.print("Location: ");
  Serial.println(datas);
  Serial.print("Critical Count: ");
  Serial.println(criticalCount);
  Serial.println("");
}
