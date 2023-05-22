#include <WString.h>
#include <Arduino.h>

#include "sensirion_common.h"
#include "sgp30.h"

class Co2VocSensor 
{
  public :

  Co2VocSensor();

  int GetCO2();
  int GetCOV();

  private :

  void measure();


  u16 scaled_ethanol_signal, scaled_h2_signal;
  u16 tvoc_ppb, co2_eq_ppm;
};
