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

#include "powertrace.h"
#include "test-flood.h"
#include "dev/leds.h"

#include <stdio.h>

#define BUF_LEN				64

#define CHUNKSIZE 		64

/*---------------------------------------------------------------------------*/
PROCESS(test_flood_process, "Broadcast example");
AUTOSTART_PROCESSES(&test_flood_process);



static char record_buf[CHUNKSIZE];
char data_buf[BUF_LEN];

uint16_t send_number = 0;
uint8_t max_package_number = 0;
uint16_t com_len = 0;
int com_fd, power_fd;

static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{
  printf("from %d.%d: package len: %d package id: %d hop num: %d\n",
         from->u8[0], from->u8[1], packetbuf_datalen(), *(char *)packetbuf_dataptr(),*(char *)(packetbuf_dataptr() + 2));

  //printf("time:%lu\n", clock_time()/CLOCK_SECOND);


  /*time package_id from hop*/
  com_len = sprintf(record_buf, "%lu %u %u %u\n", clock_time(), data_buf[0], data_buf[1], data_buf[2]);
  cfs_write(com_fd, record_buf, com_len);

  //printf("%s",record_buf);

	packetbuf_copyto(data_buf);

	leds_toggle(LEDS_BLUE);
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_flood_process, ev, data)
{
  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();

  int fd;
  int test_round = 0;
  
  char file_name[16];
  
  fd = cfs_open("rn.txt", CFS_WRITE | CFS_READ);
	if(cfs_seek(fd,0,CFS_SEEK_END) != 0)
	{
		cfs_seek(fd, 0, CFS_SEEK_SET);
		cfs_read(fd, &test_round, sizeof(test_round));
	}
	test_round++;
  cfs_seek(fd, 0, CFS_SEEK_SET);
  cfs_write(fd, &test_round, sizeof(test_round));
  cfs_close(fd);

  sprintf(file_name, "flood%d.txt", test_round);
  printf("file_name: %s\n",file_name);
  com_fd = cfs_open(file_name, CFS_WRITE);

  cfs_seek(com_fd, 0, CFS_SEEK_END);

  sprintf(file_name, "power%d.txt", test_round);
  printf("file_name: %s\n",file_name);
  power_fd = cfs_open(file_name, CFS_WRITE);
  cfs_seek(power_fd, 0, CFS_SEEK_END);


  powertrace_start(CLOCK_SECOND * 10);

  cc2420_set_channel(26);
  cc2420_set_txpower(5);

  broadcast_open(&broadcast, 129, &broadcast_call);

  while(1) {
    PROCESS_PAUSE();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

void record_power(char * buf,uint16_t len)
{
	cfs_write(power_fd, buf, len);
}


