#include "co2covSensor.h"

Co2VocSensor::Co2VocSensor()
{
  while (sgp_probe() != STATUS_OK) {
      while (1);
  }

  sgp_measure_signals_blocking_read(&this->scaled_ethanol_signal,
                                          &this->scaled_h2_signal);

  // IMPLEMENTER HUMIDITE
  
  sgp_set_absolute_humidity(50000);
  sgp_iaq_init();
}

int Co2VocSensor::GetCO2()
{
  this->measure();
  return (int)this->co2_eq_ppm;
}

int Co2VocSensor::GetCOV()
{
  this->measure();
  return (int)this->tvoc_ppb;
}


void Co2VocSensor::measure()
{
  sgp_measure_iaq_blocking_read(&this->tvoc_ppb, &this->co2_eq_ppm);
}
