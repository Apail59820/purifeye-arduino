#include "PMSensor.h"

PMSensor::PMSensor()
{
  this->sensor.init();
 
}

bool PMSensor::get_sensor_values()
{
  return this->sensor.read_sensor_value(this->buf,29);
}


int PMSensor::get_value_by_type(const int type)
{
  this->get_sensor_values();

  u16 value=0;

  if(NULL==this->buf){
    return 0;
  }
      

   int i = 0;
   switch(type)
   {
    case PM1:
      i = 5;
      break;
    case PM2p5:
      i = 6;
      break;
    case PM10:
      i = 7;
      break;
   }
   
   value = (u16)this->buf[i*2]<<8|this->buf[i*2+1];
   
   return (int)value;

}
