#ifndef SHT11_H
#define SHT11_H

void tsl2561_init(void);
void tsl2561_off(void);
/* Calculates the lux values from the calibration table
   args: raw values from sensor
   returns a lux value
*/
uint16_t calculateLux(uint16_t *readRaw);


unsigned int tsl2561_light(void);
/* Calibration settings */
#define K1T 0X0040
#define B1T 0x01f2
#define M1T 0x01be

#define K2T 0x0080
#define B2T 0x0214
#define M2T 0x02d1

#define K3T 0x00c0
#define B3T 0x023f
#define M3T 0x037b

#define K4T 0x0100
#define B4T 0x0270
#define M4T 0x03fe

#define K5T 0x0138
#define B5T 0x016f
#define M5T 0x01fc

#define K6T 0x019a
#define B6T 0x00d2
#define M6T 0x00fb

#define K7T 0x029a
#define B7T 0x0018
#define M7T 0x0012

#define K8T 0x029a
#define B8T 0x0000
#define M8T 0x0000

/* -------------------------------------------------------------------------- */


#endif 
