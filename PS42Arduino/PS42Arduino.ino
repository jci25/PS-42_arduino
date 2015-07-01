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
 
 
 
 /*
Liquid flow rate sensor -DIYhacking.com Arvind Sanjeev

Measure the liquid/water flow rate using this code. 
Connect Vcc and Gnd of sensor to arduino, and the 
signal line to arduino digital pin 2.
 
 */

byte statusLed    = 13;

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 7.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 7.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

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
 
 //flowMeter start 
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING); 
   
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
 
 
 //flowMeter Start
 
  if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    float time = millis() - oldTime; 
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    //Serial.println("*********************************************");
    // Print the flow rate for this second in litres / minute
   // Serial.print("Flow rate: ");
   // Serial.print(int(flowRate));  // Print the integer part of the variable
   // Serial.print(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac = (flowRate - int(flowRate)) * 10;
   // Serial.print(frac, DEC) ;      // Print the fractional part of the variable
   // Serial.print("L/min");
    // Print the number of litres flowed in this second
    //Serial.print("  Current Liquid Flowing: ");             // Output separator
    //Serial.print(flowMilliLitres);
    //Serial.print("mL/Sec");

    // Print the cumulative total of litres flowed since starting
    //Serial.print("  Output Liquid Quantity: ");             // Output separator
  //  Serial.print(totalMilliLitres);
    //Serial.println("mL"); 
    float inch3 = flowMilliLitres*0.0610237441; //1 mL = 0.06 cubic inches
   // Serial.print("in^3/sec = ");
    Serial.print(inch3);
    float speed = inch3 / 0.15033011721; //in^3 / sec divided by area = pi*(3.5/16 in)^2
    Serial.print(" ");
    //Serial.print("Speed in/sec: ");
    Serial.print(speed);
   // Serial.println("in/sec");
    
   // Serial.print("time (s) = "); Serial.print(millis() - oldTime); Serial.print("  ");
    //Serial.print(time);
   // Serial.print("inches = ");
   //Serial.println( (time)*speed);
   // Serial.println("*********************************************");
    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
 
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

/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
