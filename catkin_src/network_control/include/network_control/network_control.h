#ifndef __NETWORK_CONTROL_H
#define __NETWORK_CONTROL_H


#include "network_control/net_interface.h"


typedef enum Network_state
{
	NET_UNSTART = 1,
  NET_SLEEPING,
  NET_WORKING
}Network_state;

typedef enum Flight_state
{
  UAV_FLYING = 1,
  UAV_ARRIVE_POINT
}Flight_state;

void net_data_record(network_report report);



#endif