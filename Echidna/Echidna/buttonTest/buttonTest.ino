const int button = 8;
const int WAIT_TIME = 15; //seconds to push cancel button after sensors read as critical
int buttonState;
void setup() {
  // put your setup code here, to run once:
  pinMode(button,INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  critical();
}

void critical() {
  unsigned long startTime = millis();
  //turn on LED and buzzer
  
  while (millis() - startTime < (WAIT_TIME * 1000)) {
    /*MIGHT NEED TO IMPLEMENT BUTTON DEBOUNCING*/
    buttonState = digitalRead(button);

    if (buttonState == HIGH) { //if person indicates they are okay
      Serial.print("button pressed");
      return;
    }
  }
  //if button hasnt been pressed, implement emergency procedure
  Serial.print("EMERGENCY");
}
