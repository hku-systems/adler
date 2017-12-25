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

#include "gather_msg.h"

#include <stdio.h>
#include <string.h>


#define GATHER_UNICAST_CHANNEL		55




/*---------------------------------------------------------------------------*/
PROCESS(test_gather_process, "Gather test");
AUTOSTART_PROCESSES(&test_gather_process);
/*---------------------------------------------------------------------------*/
int record_fd;
void print_msg(char *payload);

static void
unicast_recv(struct unicast_conn *c, const rimeaddr_t *sender)
{
	char *msg;

	msg = packetbuf_dataptr(); 
	print_msg(msg);


   
  gather_message_record(record_fd,msg);
  leds_toggle(LEDS_BLUE);
}
/*---------------------------------------------------------------------------*/
int get_record_round(void);


static struct unicast_conn gather_uc;
static const struct unicast_callbacks unicast_call = {unicast_recv, NULL};

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_gather_process, ev, data)
{

  PROCESS_EXITHANDLER(unicast_close(&gather_uc);)
  PROCESS_BEGIN();

  char file_name[16];

  sprintf(file_name, "record%d.txt", get_record_round());
  printf("file_name: %s\n",file_name);
  record_fd = cfs_open(file_name, CFS_WRITE);

  cc2420_set_channel(26);
  cc2420_set_txpower(5);

  /* The data sink runs with a 100% duty cycle in order to ensure high
     packet reception rates. */
  NETSTACK_RDC.off(1);

  unicast_open(&gather_uc, GATHER_UNICAST_CHANNEL, &unicast_call);


  while(1) {
    /* Delay 1 seconds */
  	PROCESS_PAUSE();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

int get_record_round(void)
{
	int fd;
  int test_round = 0;

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

  return test_round;
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
