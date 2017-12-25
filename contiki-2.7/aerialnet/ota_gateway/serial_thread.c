/*
 * Copyright (c) 2016, Indian Institute of Science <http://www.iisc.ernet.in>
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
 * \addtogroup cc2538-examples
 * @{
 *
 * \defgroup cc2538-test-uart Test the CC2538 UART
 *
 * Demonstrates the use of the CC2538 UART
 *
 * @{
 *
 * \file
 *         A quick program for testing the UART1 on the cc2538 based
 * platform which can be used to interface with sensor with UART
 * interface and to communicate with other development boards
 * \author
 *         Akshay P M <akshaypm@ece.iisc.ernet.in>
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "sys/etimer.h"
#include "dev/leds.h"
#include "dev/uart1.h"
#include "dev/serial-line.h"
#include <stdio.h>
#include <stdint.h>
#include "gateway.h"

#include "serial_thread.h"


#define HEADER_0	0X55
#define HEADER_1	0XAA

typedef enum DAT_STATE
{
	DAT_INIT = 0,
	DAT_HEADER,
	DAT_LEN,
	DAT_ID,
	DAT_DATA,
	DAT_CHACK
}DAT_STATE;


#define TX_BUF_LEN			30
#define RX_BUF_LEN 			30
/*---------------------------------------------------------------------------*/
unsigned char sent_buf[TX_BUF_LEN];

process_event_t serial_event;
port_frame reveive_frame,send_frame;
RSSI_DATA rssi;
uint8_t UAV_cmd = 255;
/*---------------------------------------------------------------------------*/
PROCESS(ez240_uart_process, "ez240 uart");
/*---------------------------------------------------------------------------*/
static unsigned int uart1_send_bytes(const unsigned char *s, unsigned int len);
static int serial_sent_frame(port_frame * frame);
static unsigned char get_check(unsigned char * data_buf);
static void frame_decode(unsigned char ch);
static void data_decode(port_frame * frame);
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(ez240_uart_process, ev, data)
{
	char *rxdata;
  uint i;


  PROCESS_BEGIN();

  serial_line_init();
  uart1_set_input(serial_line_input_byte);

  serial_event = process_alloc_event();

  //etimer_set(&send_timer, CLOCK_SECOND * 4);

  while(1) 
	{
		/*PROCESS_WAIT_UNTIL(etimer_expired(&send_timer));
		sent_data(cmd_data,2);
		etimer_reset(&send_timer);
*/
    //printf("string is being sent\r");

    PROCESS_YIELD();
    if(ev == serial_line_event_message) 
    {
    	//uart1_send_bytes(cmd_data, 2);
    	leds_toggle(LEDS_RED);
    	rxdata = data;
    	//uart1_send_bytes(rxdata, 7);

	    	for(i = 0; rxdata[i] != '\0'; i++)
	      {
	          frame_decode(rxdata[i]);  
	      }
		}
		//etimer_reset(&send_timer);
	}
  PROCESS_END();
}
/*
sent_buf[2] = frame_len - 3 = frame_id~lastdata
*/
static int serial_sent_frame(port_frame * frame)
{
	unsigned int frame_len = 0,i;

	//memset(sent_buf, 0, TX_BUF_LEN);

	sent_buf[frame_len++] = HEADER_0;
	sent_buf[frame_len++] = HEADER_1;

	sent_buf[frame_len++] = 0X00;			//2 frame length

	sent_buf[frame_len++] = frame->frame_id;			//frame id 

	for (i = 0; i < frame->data_len; ++i)
	{
		sent_buf[frame_len++] = frame->data[i];
	}

	sent_buf[2] = frame_len - 3;

	sent_buf[frame_len++] = get_check(sent_buf+2);
	 
	return uart1_send_bytes(sent_buf, frame_len);


}


static void frame_decode(unsigned char ch)
{
	static DAT_STATE receive_state = DAT_INIT;
	static int data_count = 0;
	static unsigned char data_buf[RX_BUF_LEN];
	//cout << "receive_state:" << receive_state << endl;

	switch (receive_state)
	{
		case DAT_INIT:
		{
			if(ch == HEADER_0)
			{
				receive_state = DAT_HEADER;
			}
			break;
		}

		case DAT_HEADER:
		{

			if(ch == HEADER_1)
			{
				receive_state = DAT_LEN;
			}
			else
			{
				receive_state = DAT_INIT;
			}
			break;
		}
		case DAT_LEN:
		{
			data_buf[data_count++] = ch;
			reveive_frame.frame_len = ch;
			reveive_frame.data_len = reveive_frame.frame_len -1;
			receive_state = DAT_ID;
			//cout << "frame_len:" << frame_len << endl;
			break;
		}

		case DAT_ID:
		{
			data_buf[data_count++] = ch;

			reveive_frame.frame_id = ch;
			receive_state = DAT_DATA;
			break;
		}

		case DAT_DATA:
		{
			data_buf[data_count++] = ch;
			//cout << "data_count" << data_count << endl;

			if(data_count == (reveive_frame.frame_len + 1))
			{
				//cout << "frame_len" << frame_len << endl;
				receive_state = DAT_CHACK;
				data_count = 0;
			}
			break;
		}

		case DAT_CHACK:
		{
			unsigned char check_ore;
			check_ore = get_check(data_buf);
			//printf("ch :%02x  get:%02x \n",ch,check_ore);
			if(ch == check_ore)
			{
				reveive_frame.data = (char *)(data_buf + 2);
				data_decode(&reveive_frame);
				process_post(&gateway_process,serial_event,&reveive_frame);
			}
			else
			{
				printf("check error!\n");
			}
			//memset(data_buf, 0, strlen((char *)data_buf));
			receive_state = DAT_INIT;
			break;
		}

		default :
		{
			receive_state = DAT_INIT;
			break;
		}
	}
}

static void data_decode(port_frame * frame)
{
	switch(frame->frame_id)
	{
		case CMD_FRAME:
			UAV_cmd = frame->data[0];
			break;
		default:
			break;
	}
}

static void rssi_decode(port_frame * frame)
{
	rssi.time = (int *)frame->data;
	rssi.value = (int *)(frame->data + sizeof(int));
}

/*
check = (data length~last data XOR) 
*/
static unsigned char get_check(unsigned char * data_buf)
{
	int i,check_len;
	unsigned char ret = data_buf[0];
	check_len = data_buf[0] + 1;
	for(i = 1;i < check_len;i++)
	{
		ret ^= data_buf[i];
		//printf("i :%d  data:  %02x \n",i,data_buf[i]);
	}
	return ret;
}

static unsigned int
uart1_send_bytes(const unsigned char *s, unsigned int len)
{
  unsigned int i = 0;

  while(s != NULL) {
    if(i >= len) {
      break;
    }
    uart1_writeb(*s++);
    i++;
  }
  return i;
}

uint8_t get_UAV_cmd(void)
{
	return UAV_cmd;
}

void reset_UAV_cmd(void)
{
	UAV_cmd = 255;
}

/*!
return sent byte num
*/
int sent_cmd(NET_CMD cmd)
{
  char cmd_data[1];

  cmd_data[0] = cmd;

  send_frame.frame_id = CMD_FRAME;
  send_frame.data_len = 1;
  send_frame.data = cmd_data;

  return serial_sent_frame(&send_frame);
}

int sent_report(network_report * report)
{
	send_frame.frame_id = REPORT_FRAME;
  send_frame.data_len = sizeof(network_report);
  send_frame.data = (char *)report;
  return serial_sent_frame(&send_frame);
}

/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
