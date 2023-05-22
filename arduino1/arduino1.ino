#include "co2covSensor.h"
#include "PMSensor.h"

#include <SoftwareSerial.h>

Co2VocSensor *co2covSensor;
PMSensor *paticleMatterSensor;

SoftwareSerial nodeMCU (8, 7);

template<typename T> void SendData(const String name, T data) noexcept;

void setup() {
    nodeMCU.begin(9600);
  
    co2covSensor = new Co2VocSensor();

    paticleMatterSensor = new PMSensor();
}

void loop() {
  delay(500);
  SendData("CO2", co2covSensor->GetCO2());
  SendData("COV", co2covSensor->GetCOV());
  SendData("PM1", paticleMatterSensor->get_value_by_type(PMSensor::PM1));
  SendData("PM2.5", paticleMatterSensor->get_value_by_type(PMSensor::PM2p5));
  SendData("PM10", paticleMatterSensor->get_value_by_type(PMSensor::PM10));
}

template<typename T> void SendData(const String name, T data) noexcept
{
   nodeMCU.println("{" + name + "}" + "[" + String(data) + "]");
}
