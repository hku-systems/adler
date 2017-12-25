#include "collect-view.h"
#include "dev/cc2420.h"
#include "dev/leds.h"
#include "dev/tsl2561-sensor.h"
#include "dev/battery-sensor.h"
#include "dev/sht11-sensor.h"

#include "collect-view-ez240.h"

/*---------------------------------------------------------------------------*/
void
collect_view_arch_read_sensors(struct collect_view_data_msg *msg)
{

//  SENSORS_ACTIVATE(tsl2561_sensor);
  SENSORS_ACTIVATE(battery_sensor);
  SENSORS_ACTIVATE(sht11_sensor);

  msg->sensors[BATTERY_VOLTAGE_SENSOR] = battery_sensor.value(0);
  msg->sensors[BATTERY_INDICATOR] = sht11_sensor.value(SHT11_SENSOR_BATTERY_INDICATOR);
  msg->sensors[LIGHT1_SENSOR] =0;// tsl2561_sensor.value(TSL2561_SENSOR_LIGHT);
  msg->sensors[LIGHT2_SENSOR] =0;// tsl2561_sensor.value(TSL2561_SENSOR_NONE);
  msg->sensors[TEMP_SENSOR] = sht11_sensor.value(SHT11_SENSOR_TEMP);
  msg->sensors[HUMIDITY_SENSOR] = sht11_sensor.value(SHT11_SENSOR_HUMIDITY);


//  SENSORS_DEACTIVATE(tsl2561_sensor);
  SENSORS_DEACTIVATE(battery_sensor);
  SENSORS_DEACTIVATE(sht11_sensor);
}
/*---------------------------------------------------------------------------*/
