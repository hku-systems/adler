#include <stdio.h>

#include "contiki.h"
#include "cfs/cfs.h"
#include "cfs/cfs-coffee.h"
#include "dev/leds.h"

PROCESS(format_coffee_process, "Coffee format");
AUTOSTART_PROCESSES(&format_coffee_process);


PROCESS_THREAD(format_coffee_process, ev, data)
{
	PROCESS_EXITHANDLER(goto exit;)
	PROCESS_BEGIN();


	cfs_coffee_format();


	while(1) 
	{
	    static struct etimer et;
	    etimer_set(&et, CLOCK_SECOND);
	    PROCESS_WAIT_EVENT();
	    leds_on(LEDS_ALL);
	    etimer_set(&et, CLOCK_SECOND);
	    PROCESS_WAIT_EVENT();
	    leds_off(LEDS_ALL);
  	}

exit:
  	leds_off(LEDS_ALL);
	PROCESS_END();
}
