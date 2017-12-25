#include "dji_sdk/dji_sdk.h"
#include <geometry_msgs/Vector3Stamped.h>
#include "ini.h"

#include "uav_control/uav_task.h"
#include "uav_control/uav_flight_control.h"
#include "pos_read.h"

#include "network_control/network_control.h"


#define WAYPOINT_NUM		5






//typedef
typedef enum System_state
{
  INIT = 1,
  FLY_NEXT_POINT,
  DOING_MISSION,
  LANDING
}System_state;

enum UAV_fly_mode
{
  TEST_MODE = 1,
  MISSION_MODE
};


typedef struct
{
    int max_point;
} configuration;





using namespace std;

static void help(void);
static void set_mode(void);


configuration uav_config;

Waypoint mission_point[WAYPOINT_NUM];

geometry_msgs::Vector3 current_position;

uint waypoint_count = 0;

System_state running_state = INIT;
UAV_fly_mode fly_mode = TEST_MODE;
Network_state network_msg = NET_UNSTART;
Flight_state uav_stat = UAV_FLYING;


void network_state_callback(const std_msgs::UInt8::ConstPtr& msg)
{
  network_msg = (Network_state)msg->data;
}


/*!
@ret 1 success
*/
int test_menu(void)
{
  int ret = 0;
  bool takeoff_result;

  help();

  char inputChar;
  std::cin >> inputChar;

  switch (inputChar)
  {                     
    case 't':
      if(is_M100())
      {
        ROS_INFO("UAV taking off!");
        ros::spinOnce();
        UAV_get_start_location();           /*start point is ground*/
        if(!M100monitoredTakeoff())
        {
          ROS_INFO("taking off fail!");
          ret = 0;
        }
        else
        {
          ret = 1;
          running_state = FLY_NEXT_POINT;
        }
      }
      break;
    case 'g':

    	Waypoint point;

      cout << "pointX:" << endl;
      cin >> point.x;
      cout << "pointY:" << endl;
      cin >> point.y;
      point.z = FLY_HIGHT;

      uav_set_target(point);

      break;
    case 'l':
      uav_landing();
      running_state = LANDING;
      ROS_INFO("##### Start landing ....");
      break;
    case 'q':
      ros::shutdown();
      break;
    default:
      break;
  }
  return ret;
}
/*!
This function in interupt, don't block the fuction, otherwise the DJI SDK will blocked!!!
*/
void task_state_machine(void)
{
	switch(running_state)
    {

      case FLY_NEXT_POINT:
        if(!check_finished())
        {
          continue_flying();
        }
        else
        {
          if(fly_mode == TEST_MODE)
          {
            test_menu();
          }
          else if(fly_mode == MISSION_MODE)
          {
            uav_stat = UAV_ARRIVE_POINT;
            running_state = DOING_MISSION;
            UAV_state_publish(uav_stat);
            network_msg = NET_WORKING;
          }
        }
        break;
      case DOING_MISSION:
      	if(network_msg == NET_SLEEPING)
      	{
      		if(waypoint_count < WAYPOINT_NUM)
      		{
      			current_position = get_current_position();
	          cout << "current pointX:" <<current_position.x << "  pointY:" << current_position.y <<\
	          "  pointz:" << current_position.z<< endl;
	          ROS_INFO("##### Start next route %d ....", waypoint_count);
	          cout << "mission pointX:" <<mission_point[waypoint_count].x << "  pointY:" << mission_point[waypoint_count].y <<\
	          "  pointz:" << mission_point[waypoint_count].z<< endl;

      			uav_set_target(mission_point[waypoint_count++]);
	      		running_state = FLY_NEXT_POINT;
	          uav_stat = UAV_FLYING;
	          UAV_state_publish(uav_stat);
	          
      		}
      		else
      		{
      			test_menu();
      		}
      		
      	}
/*      	else
    		{
    			hovering();
    		}*/

      	break;
      case LANDING:
        if(check_landing())
        {
          test_menu();
        }
        break;
      default:
        break;
    }
}




static void help(void)
{
    cout << "|************************ current status **************************** |" << endl;

    cout << "|running_state:"<<running_state << endl;

    cout << "|************************ UAV + WSN **************************** |" << endl;
    cout << "| Available commands:                                            |" << endl;
    cout << "| [t] take off                                                   |" << endl;
    cout << "| [g] go point [x] [y]                                           |" << endl;
    cout << "| [l] landing                                                    |" << endl;
    cout << "| [m] mission mode                                               |" << endl;
    cout << "| [q] quit                                                       |" << endl;
    cout << "  input: ";
}

static void init_waypoint(void)
{
  float pos[2];
  iopos posfs;

  if(posfs.open("/home/wsn/config/pos.txt")) {
    for(uint i = 0; i < uav_config.max_point; i++) {
       posfs.read_next_pos(pos);
       mission_point[i].x = pos[0];
       mission_point[i].y = pos[1];
       mission_point[i].z = FLY_HIGHT;
    }
  }
}


static int config_handler(void* user, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("config", "max_point")) {
        pconfig->max_point = atoi(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

static bool read_config(void)
{
  if (ini_parse("/home/wsn/config/uav_config.ini", config_handler, &uav_config) < 0) {
    cout << "Can't load 'uav_config.ini'" << endl;
    return false;
  } else {
    cout << "max_point:" << uav_config.max_point << endl;
    return true;
  }
}

int task_init(void)
{
  if(read_config()) {
    init_waypoint();
    set_mode();
    return 1;
  }
  cout << "init fails!!!" << endl;
	return 0;
}

static void set_mode(void)
{
  cout << "|************************ UAV + WSN **************************** |" << endl;
  cout << "| Available commands:                                            |" << endl;
  cout << "| [t] test mode                                                  |" << endl;
  cout << "| [m] mission mode                                               |" << endl;
  cout << "  input: ";

  char inputChar;
  std::cin >> inputChar;

  switch (inputChar)
  {
    case 't':
      fly_mode = TEST_MODE;
      break;
    case 'm':
      fly_mode = MISSION_MODE;
      break;
    default:
      fly_mode = TEST_MODE;
      break;
  }
}