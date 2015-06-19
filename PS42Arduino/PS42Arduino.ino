#include <DHT.h>
#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>

//#include <OneWire.h>
//#include <DallasTemperature.h>
 
// Data wire is plugged into pin 2 on the Arduino
//#define ONE_WIRE_BUS 2
#define DHTPIN 9
#define PWMPIN 11
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
MS5803 sensor(ADDRESS_HIGH);
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
//OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
//DallasTemperature sensors(&oneWire);
//Create variables to store results
float temperature_c, temperature_f;
double depth1, pressure_abs, pressure_relative, altitude_delta, pressure_baseline;
 
void setup(void)
{
  // start serial port
  Serial.begin(9600);
  analogWrite(PWMPIN, 255);
  // Start up the library
  //sensors.begin();
  dht.begin();
  sensor.reset();
  sensor.begin();
  pressure_baseline = sensor.getPressure(ADC_2048);
}
 
 
void loop(void)
{
 // To measure to higher degrees of precision use the following sensor settings:
  // ADC_256 
  // ADC_512 
  // ADC_1024
  // ADC_2048
  // ADC_4096
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //Serial.print(" Requesting temperatures...");
 // sensors.requestTemperatures(); // Send the command to get temperatures
 // Serial.println("DONE");
 // Read temperature from the sensor in deg F. Converting
  // to Fahrenheit is not internal to the sensor.
  // Additional math is done to convert a Celsius reading.
  temperature_f = sensor.getTemperature(FAHRENHEIT, ADC_512);
  
  // Read pressure from the sensor in mbar.
  pressure_abs = sensor.getPressure(ADC_2048);
  depth1 = depth(pressure_baseline, pressure_abs);
Serial.print("WaterTemp : ");
 // Serial.print("Temperature for Device 1 is: ");
  Serial.print(temperature_f); 

  Serial.print(", Depth : ");
  Serial.print(depth1);
    // You can have more than one IC on the same bus. 
    // 0 refers to the first IC on the wire
  Serial.print(", Humidity : ");
  Serial.print((float)dht.readHumidity(), 2);

  Serial.print(", HullTemp : ");
  Serial.println((float)dht.readTemperature(true), 2);
 
}

 double depth(double A, double B)
{
  double tmp = B-A;
  if (tmp <0){
    return 0;
  }else{
    return round(tmp*.401325981);
  }
}
