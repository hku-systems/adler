#include "contiki.h"
#include <stdio.h>
#include <math.h>
//#include <dev/tsl2561.h>
#include "tsl2561.h"
#include "tsl2561-arch.h"

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#ifndef SDA_0
#define SDA_0()   (TSL2561_SDA_PxDIR |=  BV(TSL2561_ARCH_SDA))	/* SDA Output=0 */
#define SDA_1()   (TSL2561_SDA_PxDIR &= ~BV(TSL2561_ARCH_SDA))	/* SDA Input */
#define SDA_IS_1  (TSL2561_SDA_PxIN & BV(TSL2561_ARCH_SDA))

#define SCL_0()   (TSL2561_SCL_PxOUT &= ~BV(TSL2561_ARCH_SCL))	/* SCL Output=0 */
#define SCL_1()   (TSL2561_SCL_PxOUT |=  BV(TSL2561_ARCH_SCL))	/* SCL Output=1 */
#define SCL_IS_1  (TSL2561_SCL_PxIN & BV(TSL2561_ARCH_SCL))
#endif
/*
 * Should avoid infinite looping while waiting for SCL_IS_1. xxx/bg
 */
#define SCL_WAIT_FOR_1() do{}while (!SCL_IS_1)

#define delay() do{_NOP(); _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); _NOP(); _NOP();}while(0)

//#define MEASURE_LIGHT	0x70
#define TSL2561_ADDR_WR	0x52
#define TSL2561_ADDR_RD	0x53
/*#define CONTROL 0x80 
#define TIMING  0x81 
#define DATA0LOW  0x8C 
#define DATA0HIGH 0x8D 
#define DATA1LOW  0x8E 
#define DATA1HIGH 0x8F*/

/* I2C Operations. */

void i2c_start(void)
{
  SDA_1();
  delay();
  SCL_1();
  //SCL_WAIT_FOR_1();
  delay();
  SDA_0();
  delay();
}

void i2c_stop(void)
{
  SDA_0();
  delay();
  SCL_1();
  delay();
  //SCL_WAIT_FOR_1();
  SDA_1();
  delay();
}
void respons() 
{
  unsigned char i=0;
  SCL_1();
  delay();
  while((SDA_IS_1)&&(i<250))
    i++;
  SCL_0();
  delay(); 
  //printf("respons:%d\n", i);
}
/*---------------------------------------------------------------------------*/
/*
 * Return true if we received an ACK.
 */
void i2c_write(unsigned _c)
{
  unsigned char c = _c;
  int i;

  for(i = 0; i < 8; i++, c <<= 1) {
    SCL_0();   
    if(c & 0x80) {
      SDA_1();
    } else {
      SDA_0();
    }
    delay();
    SCL_1();
    delay();  
  }
  SCL_0();
  delay();
  SDA_1();
  delay();
}
/*---------------------------------------------------------------------------*/
unsigned char i2c_read()
{
  int i;
  unsigned char c = 0x00;
  SCL_0();
  delay();
  SDA_1();
  delay();
  for(i = 0; i < 8; i++) {
    c <<= 1;
    SCL_1();
    delay();
    if(SDA_IS_1) {
      c |= 0x1;
    }
    SCL_0();
    delay();
  }
  //SCL_1();
  //delay();
  //SCL_0();
  //delay();
  SDA_1();			/* Release SDA */
  delay();
  return c;
}
/*---------------------------------------------------------------------------*/
void WriteByte(unsigned char command,unsigned char data) 
{
  i2c_start();
  i2c_write(TSL2561_ADDR_WR);
  respons();
  i2c_write(command);
  respons();
  i2c_write(data);
  respons();
  i2c_stop(); 
}  
unsigned char ReadByte(unsigned char command) 
{
  unsigned char data;
  i2c_start();
  i2c_write(TSL2561_ADDR_WR);
  respons();
  i2c_write(command);
  respons();
  i2c_start();
  i2c_write(TSL2561_ADDR_RD);
  respons();
  data=i2c_read();
  respons();
  i2c_stop();
  return data; 
}  
/*---------------------------------------------------------------------------*/
/*
 * Power up the device. The device can be used after an additional
 * 11ms waiting time.
 */
void tsl2561_init(void)
{
  /*
   * SCL Output={0,1}
   * SDA 0: Output=0
   *     1: Input and pull-up (Output=0)
   */
#ifdef TSL2561_INIT
  TSL2561_INIT();
#else
  TSL2561_PWR_PxOUT |= BV(TSL2561_ARCH_PWR);
  TSL2561_SDA_PxOUT &= ~BV(TSL2561_ARCH_SDA);
  TSL2561_SCL_PxOUT &= ~BV(TSL2561_ARCH_SCL);
  TSL2561_PWR_PxDIR |= BV(TSL2561_ARCH_PWR);
  TSL2561_SCL_PxDIR |= BV(TSL2561_ARCH_SCL);
#endif
}
/*---------------------------------------------------------------------------*/
/*
 * Power off device.
 */
void
tsl2561_off(void)
{
#ifdef TSL2561_OFF
  TSL2561_OFF();
#else
  TSL2561_PWR_PxOUT &= ~BV(TSL2561_ARCH_PWR);
  TSL2561_SDA_PxOUT &= ~BV(TSL2561_ARCH_SDA);
  TSL2561_SCL_PxOUT &= ~BV(TSL2561_ARCH_SCL);
  TSL2561_PWR_PxDIR |= BV(TSL2561_ARCH_PWR);
  TSL2561_SCL_PxDIR |= BV(TSL2561_ARCH_SCL);
#endif
}
/*---------------------------------------------------------------------------*/
uint16_t calculateLux(uint16_t *buffer) //buffer ch00,ch01, ch10, ch11
{
  //Gain:1X 402ms is used
   uint32_t ch0, ch1 = 0;
  uint32_t chScale = (1<<14); //assume no scaling 1X 402ms
  uint32_t ratio, lratio, tmp = 0;

  ch0 = (buffer[0]*chScale) >> 10;
  ch1 = (buffer[1]*chScale) >> 10;

  if (ch0 != 0)
  	ratio = (ch1 << 10)/ch0;
  lratio = (ratio+1) >> 1;

  if ((lratio >= 0) && (lratio <= K1T))
    tmp = (ch0*B1T) - (ch1*M1T);
  else if (lratio <= K2T)
    tmp = (ch0*B2T) - (ch1*M2T);
  else if (lratio <= K3T)
    tmp = (ch0*B3T) - (ch1*M3T);
  else if (lratio <= K4T)
    tmp = (ch0*B4T) - (ch1*M4T);
  else if (lratio <= K5T)
    tmp = (ch0*B5T) - (ch1*M5T);
  else if (lratio <= K6T)
    tmp = (ch0*B6T) - (ch1*M6T);
  else if (lratio <= K7T)
    tmp = (ch0*B7T) - (ch1*M7T);
  else if (lratio > K8T)
    tmp = (ch0*B8T) - (ch1*M8T);

  if (tmp < 0) tmp = 0;
    
  tmp += (1<<13);
  return (tmp >> 14);
}

/*---------------------------------------------------------------------------*/

uint16_t tsl2561_light(void)
{
  unsigned char DataLow, DataHigh;
//  unsigned int channel0, channel1;
  uint16_t elight;
  uint16_t light_buf[] = {0x00, 0x00};

  unsigned char Command = 0x80;
  unsigned char Data = 0x03;
  WriteByte(Command, Data);
  Command = 0x81;
  Data = 0x02;   //1X
  WriteByte(Command, Data);

  Command = 0x8C; //Address the Ch0 lower data register
  DataLow = ReadByte (Command); //Result returned in DataLow
  Command = 0x8D; //Address the Ch0 upper data register
  DataHigh = ReadByte (Command); //Result returned in DataHigh

   light_buf[0] = (DataHigh << 8 | DataLow); //Shift DataHigh to upper byte
   
 // printf("DataHight:%d,DataLow:%d.\n", DataHigh, DataLow);

  Command = 0x8E; //Address the Ch1 lower data register
  DataLow = ReadByte (Command); //Result returned in DataLow
  Command = 0x8F; //Address the Ch1 upper data register
  DataHigh = ReadByte (Command); //Result returned in DataHigh

  light_buf[1] = (DataHigh << 8 | DataLow); //Shift DataHigh to upper byte

 // printf("DataHight:%d,DataLow:%d.\n", DataHigh, DataLow);

  //  printf("channel0:%d,channel1:%d.\n", light_buf[0], light_buf[1]);

  elight = calculateLux(&light_buf);
//  printf("elight:%d\n", (uint16_t)elight);

  return elight;
}
/*---------------------------------------------------------------------------*/

