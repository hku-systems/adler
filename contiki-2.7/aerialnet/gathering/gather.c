/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Testing the broadcast layer in Rime
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime.h"
#include "net/netstack.h"
#include "random.h"
#include "dev/cc2420.h"
#include "cfs/cfs.h"
#include "sys/node-id.h"

#include "dev/button-sensor.h"
#include "dev/leds.h"

#include "gather_msg.h"

#include <stdio.h>
#include <string.h>


#define GATHER_UNICAST_CHANNEL		55
#define SENSOR_NUM    12

#define RANDWAIT (4)

enum rout_type
{
	LINE_ROUTE = 1,
	TREE_ROUTE,
	CLUSTER_ROUTE,
	UAV_ROUTE
};


/*---------------------------------------------------------------------------*/
PROCESS(test_gather_process, "Gather test");
AUTOSTART_PROCESSES(&test_gather_process);
/*---------------------------------------------------------------------------*/
uint8_t rout_list[4][SENSOR_NUM] = {{17,18,19,20,23,24,25,27,28,31,32,34},
                                    {16,17,18,19,20,23,24,25,27,28,31,32},
                                    {16,16,17,17,18,18,19,19,20,20,23,23},
                                    {16,17,18,17,17,17,18,18,18,19,19,19}};

static struct unicast_conn gather_uc;
rimeaddr_t parent;
rimeaddr_t get_parent(uint8_t rout);
void show_rout(enum rout_type rout);
void print_msg(char *payload);

static void
unicast_recv(struct unicast_conn *c, const rimeaddr_t *sender)
{
	char *msg;

  msg = packetbuf_dataptr(); 
  //print_msg(msg);

  packetbuf_copyfrom(msg, packetbuf_datalen()); 
  unicast_send(&gather_uc, &parent);                          //forward to parent
}
/*---------------------------------------------------------------------------*/
static const struct unicast_callbacks unicast_call = {unicast_recv, NULL};

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_gather_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&gather_uc);)
  PROCESS_BEGIN();

  static struct etimer et;
  static struct gather_data_msg msg;
  static int packet_count = 0;

  static enum rout_type rout = LINE_ROUTE;

  SENSORS_ACTIVATE(button_sensor);

  cc2420_set_channel(26);
  cc2420_set_txpower(5);

  parent = get_parent(rout);
  show_rout(rout);
  unicast_open(&gather_uc, GATHER_UNICAST_CHANNEL, &unicast_call);


  while(1) {
    /* Delay RANDWAIT seconds */
  	etimer_set(&et, random_rand() % (CLOCK_SECOND * RANDWAIT));

  	PROCESS_WAIT_EVENT();
  	if(ev == sensors_event &&
		     data == &button_sensor)
  	{
  		rout++;
  		if(rout == 5)
  		{
  			rout = LINE_ROUTE;
  		}
  		printf("route: %d\n", rout);
  		parent = get_parent(rout);
  		show_rout(rout);
  	}
  	//show_rout(rout);

  	if(ev == PROCESS_EVENT_TIMER)
  	{
  		gather_construct_message(&msg, ++packet_count);

	    packetbuf_copyfrom(&msg, sizeof(msg));
	    printf("node: %d package: %d E_tx: %lu, len %d\n", msg.node_id, msg.package_id, msg.transmit, sizeof(msg));

	    unicast_send(&gather_uc, &parent);
  	}
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

rimeaddr_t get_parent(uint8_t rout)
{
  uint8_t i;
  rimeaddr_t parent;

  if(rout == 4)
  {
  	parent.u8[0] = 16;
  	parent.u8[1] = 0;
  }
  else
  {
  	for(i = 0; i < SENSOR_NUM; i++)
	  {
	    if(rout_list[0][i] == (uint8_t)node_id)
	      break;
	  }
	  parent.u8[0] = rout_list[rout][i];
	  parent.u8[1] = 0;
  }
  

  printf("my id is %d\n", node_id);

  printf("my parent is %d\n", parent.u8[0]);

  return parent;
}

void show_rout(enum rout_type rout)
{
	if(rout == LINE_ROUTE)
  	{
  		leds_on(LEDS_YELLOW);
  		leds_off(LEDS_GREEN);
  		leds_off(LEDS_RED);
  	}
  	else if (rout == TREE_ROUTE)
  	{
  		leds_on(LEDS_GREEN);
  		leds_off(LEDS_YELLOW);
  		leds_off(LEDS_RED);
  	}
  	else if (rout == CLUSTER_ROUTE)
  	{
  		leds_on(LEDS_RED);
  		leds_off(LEDS_YELLOW);
  		leds_off(LEDS_GREEN);
  	}
  	else if (rout == UAV_ROUTE)
  	{
  		leds_on(LEDS_ALL);
  	}
}

void print_msg(char *payload)
{
  uint16_t data0;
        
  memcpy(&data0, payload, sizeof(data0));
  payload += sizeof(data0);
  printf("from: %u ", data0);

  memcpy(&data0, payload, sizeof(data0));
  payload += sizeof(data0);
  printf("package_id: %u\n", data0);

  if(data0 > 150)
  {
  	leds_on(LEDS_GREEN);
  }

}

