/***************************************************
# File Name:	global.h
# Abstract:	record Global settings here
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define NDEBUG
#include <assert.h>
#include <stdint.h>
#include <string>

#define CC_ConfigPath "cc.conf"
#define ADC_ConfigPath "adc.conf"

class CDisplay;

class CC_Config
{
public:
    CC_Config();
    CC_Config(const CC_Config&);
    ~CC_Config();
    CC_Config& operator=(const CC_Config&);
    bool operator==(const CC_Config&,const CC_Config&);

private:
  uint16_t GlobalMode;
  uint16_t Delays;
  uint32_t ScalReadCtrl;
  uint32_t SelectLED;
  uint32_t SelectNIMO;
  uint32_t SelectUserDevice;
  uint32_t TimingDGGA;
  uint32_t TimingDGGB;
  uint32_t LAMMask;
  uint32_t ExtendedDelay;
  uint32_t UsbBufferSetup;

public:
  void dump();
  void dump(CDisplay*);
  void dump(std::string&);
  void clear();

  inline void setGlobalMode(uint16_t value){
      GlobalMode=value;
  }
  inline uint16_t getGlobalMode(){
      return GlobalMode;
  }
  inline void setDelays(uint16_t value){
      Delays=value;
  }
  inline uint_16 getDelays(){
      return Delays;
  }
  inline void setScalReadCtrl(uint32_t value){
      ScalReadCtrl=value;
  }
  inline uint32_t getScalReadCtrl(){
      return ScalReadCtrl;
  }
  inline void setSelectLED(uint32_t value){
      SelectLED=value;
  }
  inline uint32_t getSelectLED(){
      return SelectLED;
  }
  inline void setSelectNIMO(uint32_t value){
      SelectNIMO=value;
  }
  inline uint32_t getSelectNIMO(){
      return SelectNIMO;
  }
  inline void setSelectUserDevice(uint32_t value){
      SelectUserDevice=value;
  }
  inline uint32_t getSelectUserDevice(){
      return SelectUserDevice;
  }
  inline void setTimingDGGA(uint32_t value){
      TimingDGGA=value;
  }
  inline uint32_t getTimingDGGA(){
      return TimingDGGA;
  }
  inline void setTimingDGGB(uint32_t value){
      TimingDGGB=value;
  }
  inline uint32_t getTimingDGGB(){
      return TimingDGGB;
  }
  inline void setExtendedDelay(uint32_t value){
      ExtendedDelay=value;
  }
  inline uint32_t getExtendedDelay(){
      return ExtendedDelay;
  }
  inline void setLAMMask(uint32_t value){
      LAMMask=value;
  }
  inline uint32_t getLAMMask(){
      return LAMMask;
  }
  inline void setUsbBufferSetup(uint32_t value){
      UsbBufferSetup=value;
  }
  inline uint32_t getUsbBufferSetup(){
      return UsbBufferSetup;
  }
};

class Module_Config
{
public:
    Module_Config();
    Module_Config(const Module_Config&);
    ~Module_Config();

    Module_Config& operator=(const Module_Config&);
    bool operator==(const Module_Config&,const Module_Config&);

private:
  uint16_t UT[16];
  uint16_t LT[16];
  int16_t PED[16];
  uint16_t Ctrl;
  int station;
  std::string name;

public:
  void dump();
  void dump(CDisplay*);
  void dump(std::string&);

  inline void setName(std::string value){
      name=value;
  }
  inline void setName(const char* value){
      name=value;
  }
  inline std::string getName(){
      return name;
  }
  inline void setStation(int value){
      station=value;
  }
  inline int getStation(){
      return station;
  }
  inline void setCtrl(uint16_t value){
      Ctrl=value;
  }
  inline uint16_t getCtrl(){
      return Ctrl;
  }
  inline void setLT(int ch_id,uint16_t value){
      LT[ch_id-1]=value;
  }
  inline uint16_t getLT(int ch_id){
      return LT[ch_id-1];
  }
  inline void setUT(int ch_id,uint16_t value){
      UT[ch_id-1]=value;
  }
  inline uint16_t getUT(int ch_id){
      return UT[ch_id-1];
  }
  inline void setPED(int ch_id,int16_t value){
      PED[ch_id-1]=value;
  }
  inline int16_t getPED(int ch_id){
      return PED[ch_id-1];
  }
};

typedef std::vector<Module_Config*> ModuleConfigFactory;
typedef std::vector<NSCLmodule*>   ModuleFactory;
#endif /* _CONFIG_H_ */

