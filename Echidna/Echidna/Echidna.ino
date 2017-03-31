#include <StackArray.h> /*MIGHT NOT NEED THIS*/
#include <CurieIMU.h>

//set pin numbers:
const int power = 1;   //pin number for power switch
const int cancel = 2; //pin number for alarm cancel button
const int criticalLED = 3; //pin number for LED for critical warning
const int buzzer = 4; //pin number for buzzer
const int pulseSensor = 13; //pin number for pulse sensor
const int tempSensor = A0; //pin number for temperature sensor

//global variables
int criticalCount;
const int WAIT_TIME = 45; //seconds to push cancel button after sensors read as critical
int switchState;
int buttonState;
const int x = 5; /*MIGHT NEED TO CHANGE THIS VALUE*/
int cc;
int prevTime;
unsigned long loopTime = 0;          // get the time since program started
unsigned long interruptsTime = 0;    // get the time when free fall event is detected

void setup() {
  //initialize inputs and outputs
  pinMode(power, INPUT);
  pinMode(cancel, INPUT);
  pinMode(criticalLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  //initialize variables and states
  criticalCount = 0;
  digitalWrite(criticalLED, LOW);
  digitalWrite(buzzer, LOW);
  cc = (x * pow(10, -6)) / 30;
  prevTime = micros();

  Serial.begin(9600); // initialize Serial communication
  while (!Serial) ;   // wait for serial port to connect.

  /* Initialise the IMU */
  CurieIMU.begin();
  CurieIMU.attachInterrupt(eventCallback);

  /* Enable Free Fall Detection */
  CurieIMU.setDetectionThreshold(CURIE_IMU_FREEFALL, 1000); // 1g=1000mg
  CurieIMU.setDetectionDuration(CURIE_IMU_FREEFALL, 50);  // 50ms
  CurieIMU.interrupts(CURIE_IMU_FREEFALL);
}

void loop() {
  //loop repeatedly
  switchState = digitalRead(power);
  if (switchState == LOW) { //if power switch is off
    return;
  }
  //read monitors
  int heart = readHeartMonitor(); /*MAKE SURE OUTPUT TYPE AND INPUTS ARE SAME AS FUNCTION*/
  int temp = readTempSensor(); /*MAKE SURE OUTPUT TYPE AND INPUTS ARE SAME AS FUNCTION*/
  detectFall();
  if (isCritical(heart, temp) == HIGH) {
    criticalCount++;
  }
  else {
    criticalCount = 0;
  }
  if (criticalCount == cc) {
    emergencyProcedure();
  }

  delayMicroseconds(x); //wait a short amount of time before reading sensors again

}

int readHeartMonitor() {/*MIGHT NEED TO CHANGE OUTPUT TYPE AND POSSIBLY ADD INPUTS*/
  /*
     Pop oldest value out of list, push on new value, calculate heart rate from values in list
  */

}

int readTempSensor() { /*MIGHT NEED TO CHANGE OUTPUT TYPE AND POSSIBLY ADD INPUTS*/

}

bool isCritical(int heart, int temp) { /*CHANGE DATA TYPES TO MATCH OTHERS*/
  if (heart < 40 or heart > 220) {
    return HIGH;
  }
  if (temp) { /*FIX THIS STATEMENT FOR CRITICAL TEMPS*/
    return HIGH;
  }
  else return LOW;
}

void detectFall() { //read accelerometer to detect fall
  /*
   * MIGHT NEED THIS IF OTHER THING DOESN'T WORK
     https://www.arduino.cc/en/Tutorial/Genuino101CurieIMUOrientationVisualiser
     https://www.cs.virginia.edu/~stankovic/psfiles/bsn09-1.pdf
     MIGHT ALSO USE CURIEIMU SHOCK DETECTED FUNCTION
  */
  //detect freefall
  loopTime = millis();
  if (abs(loopTime - interruptsTime) < 1000 )
    critical();

  else
    return LOW;
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
  digitalWrite(criticalLED, HIGH);
  digitalWrite(buzzer, HIGH);
  while (millis() - startTime < (WAIT_TIME * 1000)) {
    /*MIGHT NEED TO IMPLEMENT BUTTON DEBOUNCING*/
    buttonState = digitalRead(cancel);

    if (buttonState == HIGH) { //if person indicates they are okay
      digitalWrite(criticalLED, LOW); //turn of LED and buzzer
      digitalWrite(buzzer, LOW);
      criticalCount = 0;
      return;
    }
  }
  //if button hasnt been pressed, implement emergency procedure
  emergencyProcedure();
}

void emergencyProcedure() { //user read to be in critical condition
  digitalWrite(criticalLED, LOW); //turn off LED to conserve power
  digitalWrite(buzzer, HIGH); //make sure buzzer is on so person can be located easier
  /*GPS AND SOS STUFF, MAKE SIGNAL CANCELLABLE*/
  while(1){
    /*GPS STUFF
     * IF BUTTON PRESSED: RETURN
     */
  }
}

