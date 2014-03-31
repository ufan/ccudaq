/***************************************************
# File Name:	ccu.h
# Abstract:	Head file for ccu_visit.cpp
# Author:	zhangzhelucky
# Update History:
	
****************************************************/
#ifndef _CCU_VISIT_H_
#define _CCU_VISIT_H_

#include "libxxusb.h"
#include <string>

#include "global.h"

class CCcusb
{
 public:
  CCcusb( xxusb_device_type );
  virtual ~CCcusb();

 private:
  usb_dev_handle* mDevHandle;
 public:
  std::string mSerialStr;

 private:
  bool DeviceOpen( xxusb_device_type );  // Open Current CCUsb Device
  bool DeviceClose();  // Close Current CCUsb Device

 public:
  bool Config( CC_Config );

 public:
  short CamacNAF(int,int,int);  // On Sending CAMAC NAF Commands

 private:
  int mCamacQ;
  int mCamacX;
  long mCamacData;

 private:
  // 16 Members on recording CCU's Interal Register
  long mFirmwareId;
  long mGlobalMode;
  long mDelays;
  long mScalReadCtrl;
  long mSelectLED;
  long mSelectNIMO;
  long mSelectUserDevice;
  long mTimingDGGA;
  long mTimingDGGB;
  long mLAMMask;
  long mLAM;
  long mScalerA;
  long mScalerB;
  long mExtendedDelay;
  long mUsbBufferSetup;
  long mBroadcastMap;

 public:
  // CAMAC Functions 
  bool CamacZ();  // Perform a CAMAC Initialize
  bool CamacC();  // Perform a CAMAC Clear
  bool CamacI(bool);  // Perform a CAMAC Inhibit

  int getCamacQ();
  int getCamacX();
  long getCamacData();
  bool setCamacData(long);

  // 16 Functions on Getting CCUsb Interal Register
  long getFirmwareId();
  long getGlobalMode();
  long getDelays();
  long getScalReadCtrl();
  long getSelectLED();
  long getSelectNIMO();
  long getSelectUserDevice();
  long getTimingDGGA();
  long getTimingDGGB();
  long getLAMMask();
  long getLAM();
  long getScalerA();
  long getScalerB();
  long getExtendedDelay();
  long getUsbBufferSetup();
  long getBroadcastMap();

  // 11 Functions on Setting CCUsb Interal Register
  bool setGlobalMode(long);
  bool setDelays(long);
  bool setScalReadCtrl(long);
  bool setSelectLED(long);
  bool setSelectNIMO(long);
  bool setSelectUserDevice(long);
  bool setTimingDGGA(long);
  bool setTimingDGGB(long);
  bool setLAMMask(long);
  bool setExtendedDelay(long);
  bool setUsbBufferSetup(long);

};


#endif /* _CCU_VISIT_H_ */

