/***************************************************
# File Name:	modul.h
# Abstract:	
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#ifndef _NSCLMODULE_H_
#define _NSCLMODULE_H_

#include <string>
#include <stdint.h>
#include "config.h"

class CCCUSB;

class NSCLmodule
{
 public:
  NSCLmodule(CCCUSB *);
  NSCLmodule( CCCUSB* , int );
  NSCLmodule( CCCUSB* , int , std::string );
  ~NSCLmodule(){}

 private:
  int mStation;
  std::string mName;
  CCCUSB* mCcu;

 public:
  void setStation(int);
  void setName(std::string);
  //Global configuration
  bool config(Module_Config&);
  Module_Config getConfig();

  //set control register
  bool getCtrlReg(uint16_t&);//F(6)A(0)
  bool clrCtrlReg();//F(11)A(0)
  bool setCtrlReg(uint16_t);//F(19)A(0)
  bool enableUT();//F(19)A(0)
  bool enableLT();
  bool enablePED();
  bool disableUT();//F(23)A(0)
  bool disableLT();
  bool disablePED();
  //set LT UT and PED
  bool setUT(int,uint16_t);//F(20)A(x)
  bool setLT(int,uint16_t);
  bool setPED(int,uint16_t);
  bool getUT(int,uint16_t&);//F(1)A(x)
  bool getLT(int,uint16_t&);
  bool getPED(int,uint16_t&);
  //LAM control
  bool enableLam();//F(26)A(0)
  bool disableLam();//F(24)A(0)
  bool testLam();//F(8)A(0)
  bool clrLam();//F(10)A(0)
  //Hit register
  bool getHitReg(uint16_t&);//F(6)A(1)
  bool clrHitReg();//F(11)A(1) will reset HigReg and LAM
  bool clrDataMem();//F(11)A(3) will reset HitReg,LAM and DataMemory
  //module reset
  bool reset();//F(9) same as C,Z
  //read data
  bool readDataMem(int,uint16_t&);//F(0)A(x)
  bool writeDataMem(int,uint16_t);//F(16)A(x)
  bool readSparse(int,uint16_t&);//F(4)A(0)
  //digital test
  bool setTestReg(int);//F(20)A(x)
  bool clrTestReg();//F(11)A(2)
  bool initTest();//F(25)A(0)
  //calibration
  bool initCalib1();//F(25)A(1), 1/20 full scale
  bool initCalib2();//F(25)A(2), 1/4  full scale
  
private:
  bool slctPED();  //F(17)A(0)
  bool slctUT();  //F(17)A(2)
  bool slctLT();  //F(17)A(1)
  bool slctTest(); //F(17)A(4)
  bool setParaReg(int ,uint16_t);  //F(20)A(x)
  bool getParaReg(int ,uint16_t&); //F(1)A(x)
};

#endif /* _NSCLMODULE_H_ */

