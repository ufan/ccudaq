/***************************************************
# File Name:	manager.h
# Abstract:	Head file for main.cpp
# Author:	zhangzhelucky
# Update History:
Tue Apr  9 20:02:35 2013  Defined unqiue CManager class	as a top manager of
                          the whole software 
Wed May  8 14:13:23 2013  Take it from main.h

****************************************************/

#ifndef _MANAGER_H_
#define _MANAGER_H_

#include "libxxusb.h"
#include <string>
#include "global.h"
#include <pthread.h>
#include "adc.h"
#include "ccu.h"
#include "display.h"


class CManager
{
 public:
  CManager();
  virtual ~CManager();

 private:
  short mDevNumber;  // Recrod the number of CCUsb device attached to the host
  xxusb_device_type pDeviceList[ MaxDev ];  // Pt to the CCUsb device list
  std::string mVersion;  // Software Version Number

  CAdc* pADC;
  CCcusb* pCCU;
  CDisplay* pDisplay;

  pthread_t mDisplayThread;
  pthread_t mDaqThread;  // Thread for sending data to guests

  // public:
  bool daqCycle();  // 

  CC_Config config_cc;
  ADC_Config config_adc;

 private:
  void welcome();  // Welcome Infomation after running
  bool FirstLoad();
  bool ConfigLoad();  // Load config file
  bool Config();  // Config CCU and ADC
  bool CcusbDevFind();  // On finding CCUsb Devices
  bool CcusbDevOpen();  // for openning ccusb devices indicated by User

 public:
  static std::string getTimeStr();  // get current time string

 private:
  //  bool lock_isConfiged;
  bool lock_isChecked;
  bool lock_isStarted;
  bool lock_isConfiged;
  bool lock_isDaqQuited;

  clock_t m_Daq_start, m_Daq_stop;

  void CmdAnalyse();
  static void* displayThread( void* );
  static void* daqThread( void* );

 private:
  bool isBind ;
  unsigned int m_hits;
};


#endif /* _MANAGER_H_ */


