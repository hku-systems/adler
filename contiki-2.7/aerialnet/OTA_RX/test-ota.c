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
 *         A test program for Deluge.
 * \author
 *         Nicolas Tsiftes <nvt@sics.se>
 *	   Easinet
 */

#include "contiki.h"
#include "cfs/cfs.h"
#include "deluge_rx.h"
#include "codeload.h"
#include "sys/node-id.h"
#include "dev/cc2420.h"

#include <stdio.h>
#include <string.h>

#ifndef SINK_ID
#define SINK_ID	1
#endif

#ifndef FILE_SIZE
#define FILE_SIZE 1280
#endif

#define MAX_RETRY   3

PROCESS(ota_test_process, "ota test process");
AUTOSTART_PROCESSES(&ota_test_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ota_test_process, ev, data)
{
  int fd;
  int txpower;
  static struct etimer et;
  static uint8_t try_num = 0;

  uint8_t i;

  PROCESS_BEGIN();

  cfs_coffee_format();

  //node_id_burn(20);
	printf("nodeid:%d\n",node_id);

  cc2420_set_channel(26);
  cc2420_set_txpower(CC2420_TXPOWER_MAX);
  //cc2420_set_channel(11);

	txpower = cc2420_get_txpower();
	printf("txpower:%d\n",txpower);

  fd = cfs_open("blink.ce", CFS_WRITE);

  if(cfs_seek(fd, FILE_SIZE, CFS_SEEK_SET) != FILE_SIZE) 
  {
		printf("failed to seek to the end\n");
  }

  printf("start code disseminate!!!!!!\n");

  deluge_disseminate("blink.ce", node_id == SINK_ID);
  cfs_close(fd);

  etimer_set(&et, CLOCK_SECOND * 5);

  for(;;) 
  {
	 PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
   if(check_update())
   {
      printf("new version received");


      for(i = 0;i < MAX_RETRY;i++)
      {
      	if(codeload() == 0)
        {
          printf("new version start successfully!");
          break;
        }
        else
        {
        	printf("No.%d load failed\n!",i);
        }
      }
   }
    etimer_reset(&et);
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
