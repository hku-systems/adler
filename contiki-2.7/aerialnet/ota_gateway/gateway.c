/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "serial_thread.h"
#include <stdio.h> /* For printf() */
#include "deluge_tx.h"
#include "net/rime.h"
#include "cfs/cfs.h"
#include "sys/node-id.h"
#include "dev/cc2420.h"

#define IMAGE_VERSION   1

#ifndef FILE_SIZE
#define FILE_SIZE 1280
#endif


typedef enum GATEWAY_STATE
{
	GATE_INIT = 0,
	GATE_SLEEP,
	GATE_MISSION,
}gateway_state;


void open_ota_file(char *name, uint16_t size);

/*---------------------------------------------------------------------------*/
PROCESS(gateway_process, "Gateway process");
AUTOSTART_PROCESSES(&gateway_process, &ez240_uart_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(gateway_process, ev, data)
{
  PROCESS_BEGIN();

  network_report report;
  int txpower;

  static struct etimer mission_et;
  static gateway_state state = GATE_SLEEP;

  printf("nodeid:%d\n",node_id);

  cc2420_set_channel(26);
  cc2420_set_txpower(CC2420_TXPOWER_MAX);

  txpower = cc2420_get_txpower();
  printf("txpower:%d\n",txpower);

  open_ota_file("blink.ce",FILE_SIZE);

  //etimer_set(&et, CLOCK_SECOND);
  deluge_disseminate("blink.ce", IMAGE_VERSION);

  while(1)
  {
    //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    //printf("gateway:%d uav_cmd:%d\n",state,get_UAV_cmd());

    PROCESS_YIELD();

    if(ev == serial_event)
    {
      if(state == GATE_SLEEP)
      {
        if(get_UAV_cmd() == START_MISSION)
        {
          state = GATE_MISSION;
          etimer_set(&mission_et, 60*CLOCK_SECOND);
          printf("start mission!");
        }
      }
    }
    else if (ev == deluge_event)
    {
      printf("send report to UAV");
      report.node_addr = *(rimeaddr_t *)data;
      sent_report(&report);
    }
    else if (ev == PROCESS_EVENT_TIMER)
    {
      if(data == &mission_et)
      {
        state = GATE_SLEEP;
        //deluge_stop();
        reset_UAV_cmd();
        sent_cmd(START_FLY);
      }
    }
    //etimer_reset(&et);
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/*
must do this before start deluge
*/
void open_ota_file(char *name, uint16_t size)
{
	int fd;
  fd = cfs_open(name, CFS_WRITE);
  if(fd < 0)
  {
  	printf("failed to open ota file\n");
  	return;
  }
  if(cfs_seek(fd, size, CFS_SEEK_SET) != size) 
  {
		printf("failed to seek to the end\n");
  }
}
