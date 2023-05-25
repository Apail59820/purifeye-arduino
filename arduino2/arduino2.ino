#include <SoftwareSerial.h>
#include <TH02_dev.h>

#define DHTTYPE DHT11
#define DHTPIN 2

#include "DHT.h"
#include "THSensor_base.h"

#define ENABLE_PIN 5

SoftwareSerial nodeMCU (8, 7);
DHT dht(DHTPIN, DHTTYPE);
TH02_dev TH02;
bool isReady = false;

template<typename T> void SendData(const String name, T data) noexcept;
float temp_hum_val[2] = {0};
float sensor_volt;
float RS_gas;
float ratio; 
int sensorValue = analogRead(A0);

void setup() {
  nodeMCU.begin(115200);
  Wire.begin();

  delay(150);
  dht.begin();
  TH02.begin();
}

void loop() {

   if(digitalRead(ENABLE_PIN) == HIGH){
      isReady = true;
   }
   else
   {
     isReady = false;
   }

   if(isReady)
   {
      temp_hum_val[0] = TH02.ReadTemperature();
      temp_hum_val[1] = TH02.ReadHumidity();
    
      SendData("TEMP", temp_hum_val[0]);
      SendData("HUM", temp_hum_val[1]);
    
      sensor_volt;
      RS_gas; 
      ratio; 
      sensorValue = analogRead(A0);
      sensor_volt=(float)sensorValue/1024*5.0;
      RS_gas = (5.0-sensor_volt)/sensor_volt; 
    
      ratio = RS_gas/1.31;  
    
      SendData("NO2", ratio * 30);
      
      delay(1000);
   }
}

template<typename T> void SendData(const String name, T data) noexcept
{
   nodeMCU.println("{" + name + "}" + "[" + String(data) + "]");
}
