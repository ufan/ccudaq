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

#include <string>
#include <vector>
#include "config.h"
#include "pthread.h"
#include "nsclmodule.h"
#include "CCCUSB.h"
#include "CCCUSBReadoutList.h"
#include "display.h"
//PMT testing
#include "AFG3252.h"
#include "PMTTestingConfig.h"
#include "SYX527.h"

#define PMTConfig_PATH "pmt.conf"
class CManager
{
 public:
  CManager();
  virtual ~CManager();

 private:
  std::string mVersion;  // Software Version Number
  std::string filename;

  CCCUSB* pCCU;
  ModuleFactory modules;
  CC_Config config_cc;
  ModuleConfigFactory config_module;
  CCCUSBReadoutList stacklist;
  //PMT testing
  std::string PMTdir;
  bool isPMT;
  bool isPMTConfiged;
  unsigned long packet_num;
  float I0Set;
  PMTTestingConfig config_pmt;
  HVGroup config_hv;
  SYX527* pHVController;
  std::vector<SYX527_Module*> pHVGroup;
  AFG3252* pPulser;

  bool MkDir(const char* dir,char* msg);
  bool ConfigPMT();
  bool _configAFG3252();
  bool _configSYX527();
  bool _configTesting();
  void pmtCycle();
  void delPMTConfig();

  pthread_t mPMTTestingThread;
  static void* pmtTestingThread(void*);
  //
  CDisplay* pDisplay;

  pthread_t mDisplayThread;
  pthread_t mDaqThread;  // Thread for sending data to guests

  // public:
  bool daqCycle();
  void daqInit();
  void daqClear();
  void stackStart();
  void stackStop();


 private:
  void welcome();  // Welcome Infomation after running
  bool FirstLoad();
  bool CcusbDevFind();
  bool CcusbDevOpen();
  bool ConfigLoad();  // Load config file
  bool CcudDefaultConfig();
  bool CcuLoad();
  bool ModuleLoad();
  void buildStack();
  bool Config();  // Config CCU and ADC
  void delModules();
  void delModuleConfig();

 public:
  static std::string getTimeStr();  // get current time string

 private:
  bool lock_isStarted;
  bool lock_isConfiged;
  bool lock_isDaqQuited;
  unsigned int m_hits;

  clock_t m_Daq_start, m_Daq_stop;

  void CmdAnalyse();
  static void* displayThread( void* );
  static void* daqThread( void* );

};


#endif /* _MANAGER_H_ */


