#include <stdlib.h>

#include "contiki.h"
//#include "lib/sensors.h"
#include "dev/tsl2561.h"
#include "dev/tsl2561-sensor.h"

//const struct sensors_sensor tsl2561_sensor;

enum {
  ON, OFF
};
static uint8_t state = OFF;

/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  switch(type){
  case(TSL2561_SENSOR_LIGHT):
    return tsl2561_light();
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
  switch(type) {
  case SENSORS_ACTIVE:
  case SENSORS_READY:
    return (state == ON);
  }
  return 0;
}

/*---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
  switch(type) {
  case SENSORS_ACTIVE:
    if(c) {
      if(!status(SENSORS_ACTIVE)) {
        rtimer_clock_t t0;
        tsl2561_init(); //add by shl
        state = ON;

        /* For for about 11 ms before the SHT11 can be used. */
        t0 = RTIMER_NOW();
        while(RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + RTIMER_SECOND / 10));
      }
    } else {
      tsl2561_off();
      state = OFF;
    }
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(tsl2561_sensor, "tsl2561",
	       value, configure, status);
