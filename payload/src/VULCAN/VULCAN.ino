#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>
#include <SPI.h>

Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = BUILTIN_SDCARD;

float V_0 = 5.0; // supply voltage to the pressure sensor
float rho = 1.204; // density of air 

// parameters for averaging and offset
int offset = 0;
int offset_size = 10;
int veloc_mean_size = 20;
int zero_span = 2;

// Data wire is plugged into port 20 for TC and port 40 for DS on the Arduino
#define TC_ONE_WIRE_BUS 20
#define DS_ONE_WIRE_BUS 40

int STATUS_LED = 15;
int EXT_LED = 23;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire TC_oneWire(TC_ONE_WIRE_BUS);
OneWire DS_oneWire(DS_ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature TC_sensors(&TC_oneWire);
DallasTemperature DS_sensors(&DS_oneWire);
DeviceAddress addr;

void pitotRead()
{
    float adc_avg = 0; float veloc = 0.0;
  
// average a few ADC readings for stability
  for (int ii=0;ii<veloc_mean_size;ii++){
    adc_avg+= analogRead(17)-offset;
  }
  adc_avg/=veloc_mean_size;
  
  // make sure if the ADC reads below 512, then we equate it to a negative velocity
  if (adc_avg>512-zero_span and adc_avg<512+zero_span){
  } else{
    if (adc_avg<512){
      veloc = -sqrt((-10000.0*((adc_avg/1023.0)-0.5))/rho);
    } else{
      veloc = sqrt((10000.0*((adc_avg/1023.0)-0.5))/rho);
    }
  }
  Serial.print("Velocity: ");
  Serial.println(veloc); // print velocity
  //delay(10); // delay for stability
}

void thermocoupleRead()
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  //Serial.print("Requesting temperatures from Thermocouples and Digital Sensor...");
  TC_sensors.requestTemperatures(); // Send the command to get temperatures from TC bus
  DS_sensors.requestTemperatures(); // Send the command to get temperatures from DS bus
  //Serial.println("DONE");
  
  // Read temperatures from TC sensors
  //Serial.println("Thermocouple readings:");
  for (uint8_t s = 0; s < TC_sensors.getDeviceCount(); s++) {
    // Get the unique address 
    TC_sensors.getAddress(addr, s);
    // Just look at bottom two bytes, which is pretty likely to be unique
    int smalladdr = (addr[6] << 8) | addr[7];
    
    Serial.print("Temp for TC device #"); Serial.print(s); 
    Serial.print(" with ID #"); Serial.print(smalladdr);
    Serial.print(" is: ");
    Serial.println(TC_sensors.getTempCByIndex(s));  
  }

  // Read temperatures from DS sensors
  //Serial.println("Digital Sensor readings:");
  for (uint8_t s = 0; s < DS_sensors.getDeviceCount(); s++) {
    // Get the unique address 
    DS_sensors.getAddress(addr, s);
    // Just look at bottom two bytes, which is pretty likely to be unique
    int smalladdr = (addr[6] << 8) | addr[7];
    
    Serial.print("Temp for DS device #"); Serial.print(s); 
    Serial.print(" with ID #"); Serial.print(smalladdr);
    Serial.print(" is: ");
    Serial.println(DS_sensors.getTempCByIndex(s));  
  }
}

void setup() 
{
  // start serial port
  Serial.begin(9600);

  Serial.println();Serial.println();
  Serial.println("VULCAN Startup!");
  Serial.println();Serial.println();
  delay(1000);

  // Start up the library
  TC_sensors.begin();
  DS_sensors.begin();

  // Check if sensors are connected
  if (TC_sensors.getDeviceCount() == 0) {
    Serial.println("No thermocouple sensors found!");
  }

  if (DS_sensors.getDeviceCount() == 0) {
    Serial.println("No digital sensors found!");
  }

  //set system led pins as outputs
  pinMode(STATUS_LED, OUTPUT);
  pinMode(EXT_LED, OUTPUT);

  // pitot tube: setup and calculate offset
  for (int ii=0;ii<offset_size;ii++){
    offset += analogRead(17)-(1023/2);
  }
  offset /= offset_size;

  digitalWrite(STATUS_LED, HIGH);
  delay(100);
}

void loop() {
  digitalWrite(STATUS_LED, LOW);
  pitotRead();
  thermocoupleRead();
  digitalWrite(STATUS_LED, HIGH);
  delay(50);
}
