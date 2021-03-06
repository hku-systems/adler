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
 *         Scanning 2.4 GHz radio frequencies using CC2420 and prints
 *         the values
 * \author
 *         Joakim Eriksson, joakime@sics.se
 *	   Easinet
 */

#include "contiki.h"
#include "net/rime.h"
#include "net/netstack.h"

#include "dev/leds.h"
#include "dev/cc2420.h"
#include "dev/cc2420_const.h"
#include "dev/spi.h"
#include "cfs/cfs.h"
#include <stdio.h>


#define CHUNKSIZE 64
static int record_fd;
static char record_buf[CHUNKSIZE];

/*---------------------------------------------------------------------------*/
int get_record_round(void);
/* This assumes that the CC2420 is always on and "stable" */
static void
set_frq(int c)
{
  int f;
  /* fine graied channel - can we even read other channels with CC2420 ? */
  f = c + 302 + 0x4000;

  CC2420_WRITE_REG(CC2420_FSCTRL, f);
  CC2420_STROBE(CC2420_SRXON);
}

static void
do_rssi(void)
{
  
  int p = 0, tmp;

  p += sprintf(record_buf + p, "%d ", (int)clock_time());

  set_frq(129);
  tmp = cc2420_rssi();
  p += sprintf(record_buf + p, "%d ", tmp - 45);

  set_frq(130);
  tmp = cc2420_rssi();
  p += sprintf(record_buf + p, "%d ", tmp - 45);

  set_frq(131);
  tmp = cc2420_rssi();
  p += sprintf(record_buf + p, "%d ", tmp - 45);

  p += sprintf(record_buf + p, "\n");

  cfs_write(record_fd, record_buf, p);


  /*int channel;

  printf("RSSI:");
  for(channel = 0; channel <= 79; ++channel) {
    if(channel < 72)
    {
      printf("%d ", 0);
    }
    else
    {
      set_frq(channel + 55);
      printf("%d ", cc2420_rssi() + 55);
    }
  }
  printf("\n");*/
}

/*---------------------------------------------------------------------------*/
PROCESS(rcecorder_process, "RSSI Recorder");
AUTOSTART_PROCESSES(&rcecorder_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(rcecorder_process, ev, data)
{
  char file_name[10];

  PROCESS_BEGIN();

  cc2420_set_txpower(CC2420_TXPOWER_MAX);


  sprintf(file_name, "rssi%d.txt", get_record_round());
  printf("file_name: %s\n",file_name);
  record_fd = cfs_open(file_name, CFS_WRITE);

  /* switch mac layer off, and turn radio on */
  NETSTACK_MAC.off(0);
  cc2420_on();

  while(1) {
    do_rssi();
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
