
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
 
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define DHTPIN 9
#define PWMPIN 11
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
 
void setup(void)
{
  // start serial port
  Serial.begin(9600);
  analogWrite(PWMPIN, 255);
  // Start up the library
  sensors.begin();
  dht.begin();
}
 
 
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
 // Serial.println("DONE");
Serial.print("Temp : ");
 // Serial.print("Temperature for Device 1 is: ");
  Serial.print(sensors.getTempFByIndex(0)); // Why "byIndex"? 
    // You can have more than one IC on the same bus. 
    // 0 refers to the first IC on the wire
  Serial.print(", Humidity : ");
  Serial.println((float)dht.readHumidity(), 2);
 
}
