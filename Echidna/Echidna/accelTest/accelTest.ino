#include <CurieIMU.h>
/*#include <MadgwickAHRS.h>

Madgwick filter;
unsigned long microsPerReading, microsPrevious;
float accelScale, gyroScale;*/
unsigned long loopTime = 0;          // get the time since program started
unsigned long interruptsTime = 0;    // get the time when free fall event is detected


void setup() {
  /*Serial.begin(9600);

  // start the IMU and filter
  CurieIMU.begin();
  CurieIMU.setGyroRate(25);
  CurieIMU.setAccelerometerRate(25);
  filter.begin(25);

  // Set the accelerometer range to 2G
  CurieIMU.setAccelerometerRange(2);
  // Set the gyroscope range to 250 degrees/second
  CurieIMU.setGyroRange(250);

  // initialize variables to pace updates to correct rate
  microsPerReading = 1000000 / 25;
  microsPrevious = micros();*/
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
  /*int aix, aiy, aiz;
  int gix, giy, giz;
  float ax, ay, az;
  float gx, gy, gz;
  unsigned long microsNow;

  // check if it's time to read data and update the filter
  microsNow = micros();
  if (microsNow - microsPrevious >= microsPerReading) {

    // read raw data from CurieIMU
    CurieIMU.readMotionSensor(aix, aiy, aiz, gix, giy, giz);

    // convert from raw data to gravity and degrees/second units
    ax = convertRawAcceleration(aix);
    ay = convertRawAcceleration(aiy);
    az = convertRawAcceleration(aiz);
    gx = convertRawGyro(gix);
    gy = convertRawGyro(giy);
    gz = convertRawGyro(giz);

    // update the filter, which computes orientation
    filter.updateIMU(gx, gy, gz, ax, ay, az);

    // print the heading, pitch and roll

    int a = sqrt(pow(ax,2)+pow(ay,2)+pow(az,2));
    int g = sqrt(pow(gx,2)+pow(gy,2)+pow(gz,2));
    Serial.print (a);
    Serial.print (g);
    //Serial.print("");
    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + microsPerReading;
    
  }*/
  detectFall();
}

/*float convertRawAcceleration(int aRaw) {
  // since we are using 2G range
  // -2g maps to a raw value of -32768
  // +2g maps to a raw value of 32767
  
  float a = (aRaw * 2.0) / 32768.0;
  return a;
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767
  
  float g = (gRaw * 250.0) / 32768.0;
  return g;
}*/

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
}

static void eventCallback() {
  if (CurieIMU.getInterruptStatus(CURIE_IMU_FREEFALL)) {
    Serial.println("free fall detected! ");
    interruptsTime = millis();
  }
}

void critical(){
  Serial.print("CRITICAL");
}

