#include <OneWire.h>
#include <DallasTemperature.h>

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

void setup(void)
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
}

void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures from Thermocouples and Digital Sensor...");
  TC_sensors.requestTemperatures(); // Send the command to get temperatures from TC bus
  DS_sensors.requestTemperatures(); // Send the command to get temperatures from DS bus
  Serial.println("DONE");
  
  // Read temperatures from TC sensors
  Serial.println("Thermocouple readings:");
  for (uint8_t s = 0; s < TC_sensors.getDeviceCount(); s++) {
    // Get the unique address 
    TC_sensors.getAddress(addr, s);
    // Just look at bottom two bytes, which is pretty likely to be unique
    int smalladdr = (addr[6] << 8) | addr[7];
    
    Serial.print("Temperature for TC device #"); Serial.print(s); 
    Serial.print(" with ID #"); Serial.print(smalladdr);
    Serial.print(" is: ");
    Serial.println(TC_sensors.getTempCByIndex(s));  
  }

  // Read temperatures from DS sensors
  Serial.println("Digital Sensor readings:");
  for (uint8_t s = 0; s < DS_sensors.getDeviceCount(); s++) {
    // Get the unique address 
    DS_sensors.getAddress(addr, s);
    // Just look at bottom two bytes, which is pretty likely to be unique
    int smalladdr = (addr[6] << 8) | addr[7];
    
    Serial.print("Temperature for DS device #"); Serial.print(s); 
    Serial.print(" with ID #"); Serial.print(smalladdr);
    Serial.print(" is: ");
    Serial.println(DS_sensors.getTempCByIndex(s));  
  }

  // Delay before next loop iteration
  delay(1000);
}

/*
  for (uint8_t s=0; s < TC_sensors.getDeviceCount(); s++) {
    // get the unique address 
    TC_sensors.getAddress(addr, s);
    // just look at bottom two bytes, which is pretty likely to be unique
    int smalladdr = (addr[6] << 8) | addr[7];
    
    Serial.print("Temperature for the device #"); Serial.print(s); 
    Serial.print(" with ID #"); Serial.print(smalladdr);
    Serial.print(" is: ");
    Serial.println(TC_sensors.getTempCByIndex(s));  
  }
}
*/