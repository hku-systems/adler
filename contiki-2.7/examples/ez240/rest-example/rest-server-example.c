#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest.h"

#if defined (PLATFORM_HAS_LIGHT)
#include "dev/tsl2561-sensor.h"     //by zz
#endif
#if defined (PLATFORM_HAS_BATTERY)
#include "dev/battery-sensor.h"
#endif
#if defined (PLATFORM_HAS_SHT11)
#include "dev/sht11-sensor.h"
#endif
#if defined (PLATFORM_HAS_LEDS)
#include "dev/leds.h"
#endif

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

char temp[100];

/* Resources are defined by RESOURCE macro, signature: resource name, the http methods it handles and its url*/
RESOURCE(helloworld, METHOD_GET, "helloworld");

/* For each resource defined, there corresponds an handler method which should be defined too.
 * Name of the handler method should be [resource name]_handler
 * */
void
helloworld_handler(REQUEST* request, RESPONSE* response)
{
  sprintf(temp,"Hello World!\n");

  rest_set_header_content_type(response, TEXT_PLAIN);
  rest_set_response_payload(response, (uint8_t*)temp, strlen(temp));
}

RESOURCE(discover, METHOD_GET, ".well-known/core");
void
discover_handler(REQUEST* request, RESPONSE* response)
{
  char temp[100];
  int index = 0;
  index += sprintf(temp + index, "%s,", "</helloworld>;n=\"HelloWorld\"");
#if defined (PLATFORM_HAS_LEDS)
  index += sprintf(temp + index, "%s,", "</led>;n=\"LedControl\"");
  index += sprintf(temp + index, "%s,", "</toggle>;n=\"LedToggle\"");
#endif

#if defined (PLATFORM_HAS_BATTERY)
  index += sprintf(temp + index, "%s", "</battery>;n=\"Battery\"");
#endif

#if defined (PLATFORM_HAS_LIGHT1)
  index += sprintf(temp + index, "%s", "</light>;n=\"Light\"");
#endif

  rest_set_response_payload(response, (uint8_t*)temp, strlen(temp));
  rest_set_header_content_type(response, APPLICATION_LINK_FORMAT);
}

#if defined (PLATFORM_HAS_BATTERY)
  uint16_t battery_sen;

void
read_battery_sensor(uint16_t* battery_1)
{
	uint16_t bateria = battery_sensor.value(0);
	uint16_t mv1000= (bateria * 5)/4096 ;
	uint16_t tempmv=bateria-(mv1000*4096/5);
	uint16_t mv100=(tempmv* 50)/4096;
	tempmv= tempmv-(mv100*4096)/50;
	uint16_t mv10=(tempmv* 500)/4096;
	tempmv=tempmv-(mv10*4096)/500;
	uint16_t mv=(tempmv* 5000)/4096;
	*battery_1=mv1000*1000+mv100*100+mv10*10+mv;
}

RESOURCE(battery, METHOD_GET, "battery");
void
battery_handler(REQUEST* request, RESPONSE* response)
{
//  read_light_sensor(&light_photosynthetic, &light_solar);
  read_battery_sensor(&battery_sen);
//  sprintf(temp,"%u;%u", light_photosynthetic, light_solar);
  sprintf(temp,"%u mv", battery_sen);

  char etag[4] = "ABCD";
  rest_set_header_content_type(response, TEXT_PLAIN);
  rest_set_header_etag(response, etag, sizeof(etag));
  rest_set_response_payload(response, temp, strlen(temp));
}
#endif

#if defined (PLATFORM_HAS_LIGHT1)
//uint16_t light_photosynthetic;
//uint16_t light_solar;
  uint16_t light_total;
/*
void
read_light_sensor(uint16_t* light_1, uint16_t* light_2)
{
  *light_1 = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
  *light_2 = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
}*/

void
read_light_sensor(uint16_t* light_1)
{
  *light_1 = tsl2561_sensor.value(TSL2561_SENSOR_LIGHT);

}


/*A simple getter example. Returns the reading from light sensor with a simple etag*/
RESOURCE(light, METHOD_GET, "light");
void
light_handler(REQUEST* request, RESPONSE* response)
{
//  read_light_sensor(&light_photosynthetic, &light_solar);
  read_light_sensor(&light_total);
//  sprintf(temp,"%u;%u", light_photosynthetic, light_solar);
  sprintf(temp,"%u", light_total);

  char etag[4] = "ABCD";
  rest_set_header_content_type(response, TEXT_PLAIN);
  rest_set_header_etag(response, etag, sizeof(etag));
  rest_set_response_payload(response, temp, strlen(temp));
}
#endif /*PLATFORM_HAS_LIGHT*/

#if defined (PLATFORM_HAS_LEDS)
/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
RESOURCE(led, METHOD_POST | METHOD_PUT , "led");

void
led_handler(REQUEST* request, RESPONSE* response)
{
  char color[10];
  char mode[10];
  uint8_t led = 0;
  int success = 1;

  if (rest_get_query_variable(request, "color", color, 10)) {
    PRINTF("color %s\n", color);

    if (!strcmp(color,"red")) {
      led = LEDS_RED;
    } else if(!strcmp(color,"green")) {
      led = LEDS_GREEN;
    } else if ( !strcmp(color,"yellow") ) {
      led = LEDS_YELLOW;
    } else {
      success = 0;
    }
  } else {
    success = 0;
  }

  if (success && rest_get_post_variable(request, "mode", mode, 10)) {
    PRINTF("mode %s\n", mode);

    if (!strcmp(mode, "on")) {
      leds_on(led);
    } else if (!strcmp(mode, "off")) {
      leds_off(led);
    } else {
      success = 0;
    }
  } else {
    success = 0;
  }

  if (!success) {
    rest_set_response_status(response, BAD_REQUEST_400);
  }
}


/*A simple actuator example. Toggles the red led*/
RESOURCE(toggle, METHOD_GET | METHOD_PUT | METHOD_POST, "toggle");
//RESOURCE(toggle, METHOD_PUT, "toggle");

void
toggle_handler(REQUEST* request, RESPONSE* response)
{
  leds_toggle(LEDS_RED);
}
#endif /*defined (CONTIKI_HAS_LEDS)*/


PROCESS(rest_server_example, "Rest Server Example");
AUTOSTART_PROCESSES(&rest_server_example);

PROCESS_THREAD(rest_server_example, ev, data)
{
  PROCESS_BEGIN();

#ifdef WITH_COAP
  PRINTF("COAP Server\n");
#else
  PRINTF("HTTP Server\n");
#endif

  rest_init();

#if defined (PLATFORM_HAS_BATTERY)
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource(&resource_battery);
#endif

#if defined (PLATFORM_HAS_LIGHT1)
//  SENSORS_ACTIVATE(light_sensor);
  SENSORS_ACTIVATE(tsl2561_sensor);
  rest_activate_resource(&resource_light);
#endif
#if defined (PLATFORM_HAS_LEDS)
  rest_activate_resource(&resource_led);
  rest_activate_resource(&resource_toggle);
#endif /*defined (PLATFORM_HAS_LEDS)*/

  rest_activate_resource(&resource_helloworld);
  rest_activate_resource(&resource_discover);

  PROCESS_END();
}
