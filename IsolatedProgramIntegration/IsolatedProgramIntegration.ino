//Things to include at top of program:

#include <PulseSensorBPM.h>

const int PIN_INPUT = A0;
const unsigned long MICROS_PER_READ = 2 * 1000L;
const boolean REPORT_JITTER_AND_HANG = true;
const long OFFSET_MICROS = 1L;
unsigned long wantMicros;
long minJitterMicros;
long maxJitterMicros;
unsigned long lastReportMicros;
byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 20;
int BPM; //This will have the value of the Beats Per Minute of the person's heart rate.
PulseSensorBPM pulseDetector(PIN_INPUT, MICROS_PER_READ / 1000L);


//Run this in the beginning of the program, or put in your setup() function.

void setup() 
{
  Serial.begin(115200); //Use if you want to print out BPM
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
  lastReportMicros = 0L;
  resetJitter();
  wantMicros = micros() + MICROS_PER_READ;
}

//This is the main body of the program. Add this to your loop function.

void loop() 
{
  unsigned long nowMicros = micros();
  if ((long) (wantMicros - nowMicros) > 1000L) 
  {
    return;
  }
  
  if ((long) (wantMicros - nowMicros) > 3L + OFFSET_MICROS) 
  {
    delayMicroseconds((unsigned int) (wantMicros - nowMicros) - OFFSET_MICROS);
    nowMicros = micros();    
  }
  
  long jitterMicros = (long) (nowMicros - wantMicros);
  
  if (minJitterMicros > jitterMicros) 
  {
    minJitterMicros = jitterMicros;
  }
  
  if (maxJitterMicros < jitterMicros) 
  {
    maxJitterMicros = jitterMicros;
  }
  
  wantMicros = nowMicros + MICROS_PER_READ;
  boolean QS = pulseDetector.readSensor();
  
  if (--samplesUntilReport == (byte) 0) 
  {
    samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
  }
  
  if (QS) 
  {
    BPM = pulseDetector.getBPM();
    Serial.print(BPM); //Use if you want to print BPM
    Serial.println(); //Use if you want to print BPM
  }
}

//Put this somewhere in the program.

void resetJitter() 
{
  // min = a number so large that any value will be smaller than it;
  // max = a number so small that any value will be larger than it.
  minJitterMicros = 60 * 1000L;
  maxJitterMicros = -1;
}

