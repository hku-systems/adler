#include <iostream>
#include <stdio.h>
#include <fstream>
#include <pthread.h>
#include <time.h>


//ros
#include "ros/ros.h"
#include "std_msgs/UInt8.h"
#include "network_control/network_control.h"



//defines
#define SIM_NETGATE       0


//typedef



//global variable
ros::Publisher network_pub;
ros::Subscriber uav_state_sub;

unsigned char uav_state = UAV_FLYING;
Network_state net_state = NET_SLEEPING;


// namespace
using namespace std;


port_frame frame;
ofstream log_fs;

void * read_thread(void * arg);
int sent_cmd(NET_CMD cmd);
static void net_data_record_init(void);


#if SIM_NETGATE
#define GATE_STATE        sim_gate_stat
uint8_t sim_gate_stat = 255;
void timerCallback(const ros::TimerEvent& event)
{
  sim_gate_stat = 1;
  cout << "timer fired!"<<endl;
}
#else
#define GATE_STATE        0
#endif

void uav_state_callback(const std_msgs::UInt8::ConstPtr& msg)
{
	uav_state = msg->data;
}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "network_control");
  ros::NodeHandle nh;

  std_msgs::UInt8 net_msg;
  pthread_t r_thread;

  //uint sent_count = 0;


  network_pub = nh.advertise<std_msgs::UInt8>("/network/control", 10);
  uav_state_sub = nh.subscribe("/uav_control/control", 10, uav_state_callback);

#if SIM_NETGATE 
  ros::Timer timer = nh.createTimer(ros::Duration(5), timerCallback,true);   //one-shot timer
  timer.stop();
#else
  open_port();
  net_data_record_init();
  pthread_create(&r_thread,NULL,read_thread, 0);
#endif

  
  
  ros::Rate loopRate(50);

  while(ros::ok())
  {
  	ros::spinOnce();

/*    if(uav_state == UAV_FLYING)
    {
      if(sent_count++ % 300 == 0)   //SIM uav state  fly 6s
      {
        uav_state = UAV_ARRIVE_POINT;
      }
    }*/
    
  	switch(net_state)
  	{
  		case NET_SLEEPING:
  			if(uav_state == UAV_ARRIVE_POINT)
		  	{
          cout <<"start mission!"<<endl;
		  		sent_cmd(START_MISSION);
          reset_gateway_cmd();
		  		net_state = NET_WORKING;
          net_msg.data = net_state;
          network_pub.publish(net_msg);
#if SIM_NETGATE 
          sim_gate_stat = 0;
          timer.stop();
          timer.start();
#endif
		  	}
  			break;
  		case NET_WORKING:
  			if(get_gateway_cmd() == START_FLY || GATE_STATE)
  			{
  				net_state = NET_SLEEPING;
  				net_msg.data = net_state;
  				network_pub.publish(net_msg);

          cout << "go next point!" << endl;

          //sent_count = 1;
          uav_state = UAV_FLYING;
  			}
  			break;
  		default:
  			break;
  	}
  	loopRate.sleep();
  }
}

void * read_thread(void * arg)          //read meassage from port
{
  ros::Rate looprate(50);
    while(1)
    {
        port_receive();
        looprate.sleep();
    }
}

/*!
return sent byte num
*/
int sent_cmd(NET_CMD cmd)
{
  unsigned char cmd_data[1];

  cmd_data[0] = cmd;

  frame.frame_id = CMD_FRAME;
  frame.data_len = 1;
  frame.data = cmd_data;

  return port_sent_frame(&frame);
}


static void net_data_record_init(void)
{
	char file_path[256];
	time_t tt = time(NULL);
  tm* t = localtime(&tt);
  sprintf(file_path, "/home/wsn/Documents/Record-data/%d-%02d-%02d_%02d-%02d.txt", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min);

  log_fs.open(file_path, std::ofstream::out | std::ofstream::app);
  if (!log_fs)
  {
      cout << "Error: Could not open log file!" << endl;
  }
}

void net_data_record(network_report report)
{
	log_fs<< "node " << report.node_addr << endl;
}

