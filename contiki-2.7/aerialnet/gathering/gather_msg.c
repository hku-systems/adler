#include "contiki.h"
#include "net/rime.h"
#include "net/rime/timesynch.h"
#include "gather_msg.h"
#include "dev/leds.h"
#include "sys/node-id.h"
#include "cfs/cfs.h"

#include <string.h>
#include <stdio.h>



/*---------------------------------------------------------------------------*/
void
gather_construct_message(struct gather_data_msg *msg,
                               uint16_t package_id)
{
  unsigned long cpu, lpm, transmit, listen;


  msg->clock = clock_time();
  msg->node_id = node_id;
  msg->package_id = package_id;

  energest_flush();

  cpu = energest_type_time(ENERGEST_TYPE_CPU);
  lpm = energest_type_time(ENERGEST_TYPE_LPM);
  transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
  listen = energest_type_time(ENERGEST_TYPE_LISTEN);

  msg->cpu = cpu;
  msg->lpm = lpm;
  msg->transmit = transmit;
  msg->listen = listen;


  memset(msg->sensors, 0, sizeof(msg->sensors));
}
/*---------------------------------------------------------------------------
/ need open fd in advance
---------------------------------------------------------------------------*/
void
gather_message_record(int fd,
                    char *payload)
{
	char buf[128];
  uint16_t data0;
  uint32_t data1;
  int i, len = 0;
  /*id,time*/
  for(i = 0; i < 3; i++) {					
    memcpy(&data0, payload, sizeof(data0));
    payload += sizeof(data0);
    len += sprintf(buf+len," %u", data0);
  }

  /*energy*/
  for(i = 0; i < 4; i++) {
    memcpy(&data1, payload, sizeof(data1));
    payload += sizeof(data1);
    len += sprintf(buf+len," %lu", data1);
  }

  /*sensor*/
  for(i = 0; i < 10; i++) {
    memcpy(&data0, payload, sizeof(data0));
    payload += sizeof(data0);
    len += sprintf(buf+len," %u", data0);
  }
  len += sprintf(buf+len," %lu", clock_seconds());

  len += sprintf(buf+len,"\n");

  //printf("%s",buf);

  cfs_write(fd, buf, len);

  leds_toggle(LEDS_ALL);
}


