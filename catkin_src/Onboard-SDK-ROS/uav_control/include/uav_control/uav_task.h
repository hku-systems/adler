#ifndef __UAV_TASK_H
#define __UAV_TASK_H

#include "std_msgs/UInt8.h"

#define FLY_HIGHT     10
#define FLY_SPEED     2



void network_state_callback(const std_msgs::UInt8::ConstPtr& msg);
int test_menu(void);
void task_state_machine(void);
int task_init(void);

#endif


