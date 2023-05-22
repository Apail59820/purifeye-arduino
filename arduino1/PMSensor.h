#include "Seeed_HM330X.h"
#include <HardwareSerial.h>

class PMSensor
{
  public :

    enum PMTYPES { PM1, PM2p5, PM10 };
     
    PMSensor();

    bool get_sensor_values();
    int parse_result_value(u8 *data);
    int get_value_by_type(const int type);

  private :  
  
    HM330X sensor;
    u8 buf[30];
};
