/***************************************************
# File Name:	global.h
# Abstract:	record Global settings here
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define NDEBUG
#include <assert.h>

#define false 0
#define true !false
#define MaxDev 1  // max number of devices permitted

#define CC_ConfigPath "cc.conf"
#define ADC_ConfigPath "adc.conf"

struct CC_Config
{
  long GlobalMode;
  long Delays;
  long ScalReadCtrl;
  long SelectLED;
  long SelectNIMO;
  long SelectUserDevice;
  long TimingDGGA;
  long TimingDGGB;
  long LAMMask;
  long ExtendedDelay;
  long UsbBufferSetup;

};

struct ADC_Config
{
  short Para[16][3];
  short Ctrl;
  int station;
};

struct ChlData
{
  bool sIsWrite;
  short sData;
};

struct sParameter
{
  short UT;
  short LT;
  short PED;
};

typedef struct sParameter sParm;


#endif /* _GLOBAL_H_ */

