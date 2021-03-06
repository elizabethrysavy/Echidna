//set pin numbers:
const int power = ;   //pin number for power switch
const int cancel = ; //pin number for alarm cancel button
/*DONT FORGET TO IMPLEMENT BUTTON DEBOUNCING*/
const int criticalLED = ; //pin number for LED for critical warning
const int speaker = ; //pin number for speaker

//global variables
int criticalCount;
const int WAIT_TIME = ; //time to push cancel button after sensors read as critical

void setup() {
  //initialize inputs and outputs
  pinMode(power, INPUT);
  pinMode(cancel, INPUT);
  pinMode(criticalLED, OUTPUT);
  pinMode(speaker, OUTPUT);

  //initialize variables
  criticalCount = 0;

}

void loop() {
  //loop repeatedly
  switchState = digitalRead(power);
  if (switchState == LOW) { //if power switch is off
    return;
  }
  /*MIGHT NEED TO ADD SOME TIMERS IN HERE SOMEWHERE*/
  //read monitors
  int heart = readHeartMonitor(); /*MAKE SURE OUTPUT TYPE AND INPUTS ARE SAME AS FUNCTION*/
  int temp = readTempSensor(); /*MAKE SURE OUTPUT TYPE AND INPUTS ARE SAME AS FUNCTION*/
  detectFall();
  if (isCritical(heart, temp) == HIGH) {
    criticalCount++; /*++ MIGHT NOT EXIST IN THIS LANGUAGE*/
  }
  if (criticalCount == 10) { /*CHANGE VALUE OF 10 TO ACTUAL VALUE */
    emergencyProcedure();
  }
  delayMicroseconds(5); //wait a short amount of time before reading sensors again

}

int readHeartMonitor() {/*MIGHT NEED TO CHANGE OUTPUT TYPE AND POSSIBLY ADD INPUTS*/

}

int readTempSensor() { /*MIGHT NEED TO CHANGE OUTPUT TYPE AND POSSIBLY ADD INPUTS*/

}

bool isCritical(int heart, int temp) { /*CHANGE DATA TYPES TO MATCH OTHERS*/
  
}

bool detectFall() { //read accelerometer to detect fall
  /*READ ACCELEROMETER AND INTERPRET DATA*/
  //if fall detected
  isCritical();
}

void critical() {
  unsigned long startTime = millis();
  //turn on LED and speaker
  digitalWrite(criticalLED, HIGH);
  digitalWrite(speaker, HIGH);
  while (millis() - startTime < WAIT_TIME) {
    /*MIGHT NEED TO IMPLEMENT BUTTON DEBOUNCING*/
    buttonState = digitalRead(cancel);
    if (buttonState == HIGH) { //if person indicates they are okay
      digitalWrite(criticalLED, LOW); //turn of LED and speaker
      digitalWrite(speaker, LOW);
      criticalCount = 0; /*MIGHT NOT NEED THIS IF CHANGE HOW CRITICAL IS DECIDED*/
      return;
    }
  }
  //if button hasnt been pressed, implement emergency procedure
  emergencyProcedure();
}

void emergencyProcedure() { //user read to be in critical condition
  digitalWrite(criticalLED, LOW); //turn off LED to conserve power
  digitalWrite(speaker, HIGH); //make sure speaker is on so person can be located easier
  /*GPS AND SOS STUFF*/
}

