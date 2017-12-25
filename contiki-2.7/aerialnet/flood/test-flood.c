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

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>



#define BUF_LEN				64				//no more than 128

#define MAX_PACKAGE				100

/*---------------------------------------------------------------------------*/
PROCESS(test_flood_process, "Broadcast example");
AUTOSTART_PROCESSES(&test_flood_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{
  printf("from %d.%d: package len: %d package id: %d hop num: %d\n",
         from->u8[0], from->u8[1], packetbuf_datalen(), *(char *)packetbuf_dataptr(),*(char *)(packetbuf_dataptr() + 2));
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
char data_buf[BUF_LEN];
void flood_encode(uint8_t package_id);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_flood_process, ev, data)
{
  static struct etimer et;
  static int packet_count = 0;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();

/*  record_fd = cfs_open("flood.txt", CFS_WRITE);

  cfs_seek(fd, 0, CFS_SEEK_END);*/

  cc2420_set_channel(26);
  cc2420_set_txpower(5);

  broadcast_open(&broadcast, 129, &broadcast_call);

  while(1) {

    /* Delay 1 seconds */
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    if(packet_count < MAX_PACKAGE)
    {
    	flood_encode(++packet_count);
    	packetbuf_copyfrom(data_buf, sizeof(data_buf));
	    broadcast_send(&broadcast);
	    printf("broadcast package: %d\n", packet_count);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void flood_encode(uint8_t package_id)
{
	uint8_t i;

	data_buf[0] = package_id;		//package id
	data_buf[1] = 0;						//from
	data_buf[2] = 0;		//hop count

	for(i = 3; i < BUF_LEN; i++)
	{
		data_buf[i] = i + data_buf[0] - 1;
	}
}

