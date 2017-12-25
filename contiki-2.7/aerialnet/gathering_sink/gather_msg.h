#ifndef __GATHERING_H__
#define __GATHERING_H__

#include "contiki-conf.h"

struct gather_data_msg {
  uint16_t node_id;
  uint16_t package_id;
  uint16_t clock;
  uint32_t cpu;
  uint32_t lpm;
  uint32_t transmit;
  uint32_t listen;

  uint16_t sensors[10];
};


void
gather_message_record(int fd,
                    char *payload);
void
gather_construct_message(struct gather_data_msg *msg,
                               uint16_t package_id);



#endif //__GATHERING_H__
