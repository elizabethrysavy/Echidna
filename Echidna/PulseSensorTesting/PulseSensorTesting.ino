#include <CurieIMU.h>
#include <PulseSensorBPM.h>

//set pin numbers:
const int pulseSensor = A0; //pin number for pulse sensor

//global variables
int criticalCount;
const int WAIT_TIME = 45; //seconds to push cancel button after sensors read as critical
const int x = 5; /*MIGHT NEED TO CHANGE THIS VALUE*/
int cc;
int prevTime;
unsigned long loopTime = 0;          // get the time since program started
unsigned long interruptsTime = 0;    // get the time when free fall event is detected

//pulse sensor variables

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

void setup() {
  //initialize inputs and outputs
  pinMode(LED_BUILTIN, OUTPUT);
  //initialize variables and states
  criticalCount = 0;
  digitalWrite(LED_BUILTIN, LOW);
  /* THIS MIGHT BE WRONG BECAUSE OF TYPES IDK HOW IT CONVERTS*/
  cc = (10/(x * pow(10, -6))) * 0.9; //90% of the reads taken in 30s
  prevTime = micros();

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
}

void loop() {
  //read monitors
  int heart = readHeartMonitor();
  detectFall();
  if (isCritical(heart) == HIGH) {
    criticalCount++;
    Serial.print("critical count: ");
    Serial.println(criticalCount);
  }
  else {
    if (criticalCount != 0) {
      criticalCount--;
    }
  }
  if (criticalCount >= cc) {
    critical();
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
   Serial.println(BPM);
 }
 
 return BPM;

}


bool isCritical(int heart) { 
  if (heart < 90 or (heart > 220 and heart < 300)) {
    return HIGH;
  }
  else return LOW;
}


void detectFall() { //read accelerometer to detect fall
  //detect freefall
  loopTime = millis();
  if (abs(loopTime - interruptsTime) < 1000 )
    critical();
  else
    return;
}


static void eventCallback() {
  if (CurieIMU.getInterruptStatus(CURIE_IMU_FREEFALL)) {
    Serial.println("free fall detected! ");
    interruptsTime = millis();
  }
}


void critical() {
  unsigned long startTime = millis();
  //turn on LED and buzzer
  digitalWrite(LED_BUILTIN, HIGH);
  while (millis() - startTime < (WAIT_TIME * 1000)) {
     Serial.print("CRITICAL "); //print once a second
  }
  //if button hasnt been pressed, implement emergency procedure
  emergencyProcedure();
}


void emergencyProcedure() { //user read to be in critical condition
  digitalWrite(LED_BUILTIN, LOW); //turn off LED to conserve power
  unsigned long startTime = millis();
  while(millis() - startTime < 15000){
     Serial.print("DANGER");
  }
}

void resetJitter() 
{
 // min = a number so large that any value will be smaller than it;
 // max = a number so small that any value will be larger than it.
 minJitterMicros = 60 * 1000L;
 maxJitterMicros = -1;
}

