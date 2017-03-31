int sensorPin = A1; //Pin that temp sensor is reading from

//set these values!
const int critical_temp_high = 103; //crictical temperature, high end
const int critical_temp_low = 95; //critical temperature, low end
const int critical_num = 30; //number of times that a critical temperature must be read to flag a problem, in seconds

int temp; //current temperature read
int num; //number of times the temperature has been above/below critical value

void setup()
{
  Serial.begin(9600); //baud rate is 9600. Use this if you want to print stuff for testing
}

void loop()
{
  readTemp(); //reads current temperature

  if (temp > critical_temp_high) //if temp is too high
  {
    num++; //increase number of times that temperature has been too high
  }
  else if (temp < critical_temp_low) //if temp is too low
  {
    num++; //increase number of times that temperature has been too low
  }
  else // if temp is normal
  {
    num = 0; //reset number
  }

  if (num == critical_num) //if the temp has been critical for a certain amount of time
  {
    emergencyProcedure();
  }
  delay(1000);// waits before reading again, 1 second
}

void readTemp() //function to read temperature
{
  int reading = analogRead(sensorPin); //reads the value on the pin

  //converts reading into a voltage
  float voltage = reading * 5.0; 
  voltage /= 1024.0;

  Serial.print(voltage); //use if you want print stuff
  Serial.println(" volts"); //use if you want to print stuff

  //converts reading to celsius
  float temperatureC = (voltage - 0.5) * 100; 

  Serial.print(temperatureC); //use if you want to print stuff
  Serial.println(" degrees C"); //use if you want to print stuff

  //converts reading to fahrenheit
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  
  Serial.print(temperatureF); //use if you want to print stuff
  Serial.println(" degrees F"); //use if you want to print stuff

  temp = (int) temperatureF; 
}

