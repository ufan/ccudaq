/***************************************************
# File Name:	adc.h
# Abstract:	Head file for adc_visit.cpp
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#ifndef _ADC_H_
#define _ADC_H_

#include "modul.h"

class CAdc : public CModul
{
 public:
  CAdc( CCcusb* , int );
  CAdc( CCcusb* , int , std::string );
  virtual ~CAdc(){};

 public:
  ChlData mDataReg[16];

 public:
  short getCtrlReg();
  bool setCtrlReg(short);
  bool enableUT();
  bool disableUT();
  bool enableLT();
  bool disableLT();
  bool enablePED();
  bool disablePED();
  
  unsigned short getHitReg();  // Check the Hit Register, as F(6)A(1)
  unsigned int DaqCycle( bool* );
  
  bool clrCtrlReg();  // As F(11)A(0)
  bool clrHitReg_LAM();  // As F(11)A(1)
  bool clrHitReg_LAM_Data();  // As F(11)A(3)

  bool getParaReg(int,sParm*);
  bool getParaRegAll(sParm*);
  bool setParaUT(int,short);
  bool setParaLT(int,short);
  bool setParaPED(int,short);

 private:
  short SparseRead();  // Read sparse data, as F(4)A(0)
  bool slctPED();  // Select pedestal
  bool slctUT();  // Select upper threshold
  bool slctLT();  // Select lower threshold
  bool setParaReg(int ,long);  // As F(20)
  //  bool waitEvent(int timeout);

 public:
  bool config( ADC_Config );

};

#endif /* _ADC_H_ */

