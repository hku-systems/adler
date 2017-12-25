#include "contiki.h"
#include "dev/tsl2561-sensor.h"
#include "dev/sht11-sensor.h"
#include "dev/battery-sensor.h"
#include <stdio.h>



float
floor(float x)
{
  if(x >= 0.0f) {
    return (float) ((int) x);
  } else {
    return (float) ((int) x - 1);
  }
}

static int
test_sht11(void)
{
    static unsigned rh;
    printf("Temperature:   %u degrees Celsius\n",
	(unsigned) (-39.60 + 0.01 * sht11_sensor.value(SHT11_SENSOR_TEMP)));
    rh = sht11_sensor.value(SHT11_SENSOR_HUMIDITY);
    printf("Rel. humidity: %u%%\n",
	(unsigned) (-4 + 0.0405*rh - 2.8e-6*(rh*rh)));
    return 0;
}

static int
test_battery(void)
{
    int bateria = battery_sensor.value(0);
    float mv = (bateria * 2.500 * 2) / 4096;
    printf("Battery: %i (%ld.%03d V)\n", bateria, (long) mv,
	   (unsigned) ((mv - floor(mv)) * 1000));	
    return 0;
}

PROCESS(test_sensors_process, "Test sensors Demo");
AUTOSTART_PROCESSES(&test_sensors_process);

PROCESS_THREAD(test_sensors_process, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

  SENSORS_ACTIVATE(tsl2561_sensor);
  SENSORS_ACTIVATE(sht11_sensor);
  SENSORS_ACTIVATE(battery_sensor);
  for (etimer_set(&et, CLOCK_SECOND*3);; etimer_reset(&et)) {
    PROCESS_YIELD();
    printf("Light:%d Lux\n", tsl2561_sensor.value(TSL2561_SENSOR_LIGHT));
    test_sht11();
    test_battery();
    printf("============== \n");
    printf("\n");
    
  }

  PROCESS_END();
}
