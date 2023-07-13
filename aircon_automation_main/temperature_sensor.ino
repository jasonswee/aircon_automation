#include "DHT.h"

#define DHTTYPE DHT11




const int humidity_closet_pin = 33;

int humidity_correction_closet = 47.0;
int temperature_correction_closet = 26.50;

// Initialize DHT sensor.
DHT dht_closet(humidity_closet_pin, DHTTYPE); 


float get_temperature(){
  Serial.println("Temperature: " + String(dht_closet.readTemperature()));
  float temperature=dht_closet.readTemperature() + temperature_correction_closet;
  // 
  return temperature;
}

float get_humidity(){
  float humidity=dht_closet.readHumidity() + humidity_correction_closet;
  return humidity;
}

void init_temperature_sensor(){
  pinMode(humidity_closet_pin, INPUT);
  digitalWrite(humidity_closet_pin, HIGH);
  dht_closet.begin();
}
