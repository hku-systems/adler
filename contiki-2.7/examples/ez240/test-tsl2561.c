#include "contiki.h"
#include "dev/tsl2561-sensor.h"
//#include "dev/tsl2561.h"

#include <stdio.h>

PROCESS(test_tsl2561_process, "TSL2561 test");
AUTOSTART_PROCESSES(&test_tsl2561_process);

PROCESS_THREAD(test_tsl2561_process, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

  SENSORS_ACTIVATE(tsl2561_sensor);
  for (etimer_set(&et, CLOCK_SECOND);; etimer_reset(&et)) {
    PROCESS_YIELD();
    //SENSORS_ACTIVATE(tsl2561_sensor);
    printf("TSL2561 LIGHT:%d Lux\n", tsl2561_sensor.value(TSL2561_SENSOR_LIGHT));
    //SENSORS_DEACTIVATE(tsl2561_sensor);
  }

  PROCESS_END();
}
