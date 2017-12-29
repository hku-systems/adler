## Adler

>Adler is a testbed for wireless network system combining sensor networks and UAV. Plenty of network applications can implement on Adler, 

## Code Structure
### UAV 
The UAV code are running on UAV airbrone computer base on DJI On-board SDK-ROS. We design flight control ROS node ans network control ROS node. The codes build throught catkin build system.

### Wireless Sensor Nodes
The sensor codes are running on terrestrial base on contiki os (contiki 2.7). It communicate with UAV via wireless communication. Currently we using Zigbee stack. 

