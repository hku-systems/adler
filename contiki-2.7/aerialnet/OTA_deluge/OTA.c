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
#include "deluge.h"
#include "codeload.h"
#include "sys/node-id.h"

#include <stdio.h>
#include <string.h>

#ifndef SINK_ID
#define SINK_ID	1
#endif

#ifndef FILE_SIZE
#define FILE_SIZE 1260
#endif

PROCESS(ota_test_process, "ota test process");
AUTOSTART_PROCESSES(&ota_test_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ota_test_process, ev, data)
{
  int fd;
  static struct etimer et;

  PROCESS_BEGIN();

	printf("nodeid:%d\n",node_id);

  fd = cfs_open("blink.ce", CFS_WRITE);

  if(cfs_seek(fd, FILE_SIZE, CFS_SEEK_SET) != FILE_SIZE) 
  {
		printf("failed to seek to the end\n");
  }

  deluge_disseminate("blink.ce", node_id == SINK_ID);
  cfs_close(fd);

  etimer_set(&et, CLOCK_SECOND * 5);

  for(;;) 
  {
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		if(check_update())
		{
			printf("new version received");
			if(codeload() == 0)
			{
				printf("new version start successfully!");
			}
			else
			{
				printf("bad elf!");
			}
		}
		else
		{
			etimer_reset(&et);
		}
		/*if(node_id != SINK_ID) {
		  fd = cfs_open("blink.ce", CFS_READ);
		  if(fd < 0) {
			printf("failed to open the code file\n");
		  } else {
			size = cfs_seek(fd,0,CFS_SEEK_END);
			printf("code size:%d\n", size);
					cfs_close(fd);
		  }
		}*/
		
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
