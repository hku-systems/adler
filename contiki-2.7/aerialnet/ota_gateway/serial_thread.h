#ifndef __SERIAL_THREAD_H
#define __SERIAL_THREAD_H

#include "contiki.h"
#include "net/rime.h"


typedef struct port_frame
{
  uint8_t frame_id;
  uint8_t frame_len;		//frame_id~lastdata
  uint8_t data_len;
  char *data;
}port_frame;


typedef enum NET_CMD
{
  START_MISSION = 1,
  STOP_MISSION,
  START_FLY,
  STOP_FLY
}NET_CMD;

typedef enum FRRAME_ID
{
  CMD_FRAME = 1,
  REPORT_FRAME,
}FRRAME_ID;


typedef struct network_report
{
	rimeaddr_t node_addr;
}network_report;

typedef struct RSSI_DATA
{
  int time;
  int value;
  float pos[4];
}RSSI_DATA;





PROCESS_NAME(ez240_uart_process);

extern process_event_t serial_event;


int sent_cmd(NET_CMD cmd);
uint8_t get_UAV_cmd(void);
void reset_UAV_cmd(void);
int sent_report(network_report * report);





#endif


