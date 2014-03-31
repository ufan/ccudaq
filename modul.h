/***************************************************
# File Name:	modul.h
# Abstract:	
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#ifndef _MODUL_H_
#define _MODUL_H_

#include <string>
#include "ccu.h"
#include "global.h"

class CModul
{
 public:
  CModul( CCcusb* , int );
  CModul( CCcusb* , int , std::string );
  virtual ~CModul(){};

 public:
  int mStation;
  std::string mName;
  CCcusb* mCcu;

 public:
  bool enableLam();
  bool disableLam();
  bool testLam();
  bool clrLam();
  
  long getReg1(int);
  bool setReg1(int,long);
  bool clrReg1(int a=0 );

  long getReg2(int);
  bool setReg2(int,long data=0);
  bool clrReg2(int a=0 );

 public:
  bool getReg1All(long*);
  bool getReg2All(long*);
  //  bool setRegAuto();
  bool clrReg1All();
  bool clrReg2All();

};

#endif /* _MODUL_H_ */

