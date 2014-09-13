/***************************************************
# File Name:	adc.cpp
# Abstract:	
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#include "adc.h"

#include <iostream>
using namespace std;

//extern void SendData(int);

CAdc::CAdc( CCcusb* ccu ,int n ) : CModul( ccu , n )
{
  mName = "Phillips_Adc";
  for (int i = 0; i < 16; ++i)
    {
      mDataReg[i].sIsWrite = false;
      mDataReg[i].sData = 0;
    }
}


unsigned int CAdc::DaqCycle( bool* isStarted)
{
  unsigned int n=0;  // Record how many set of data read
  enableLam();
  
  while( *isStarted )
    {
      if( false == testLam() )
	{
	  continue;
	}
      else
	{
	  SparseRead();	  
	}

      ++n;
    }
  return n;
}


short CAdc::SparseRead()
{
  short n=0;
  short channel = -1;
  short flag;
  //  cout << getHitReg() << endl;
  for (int i = 0; i < 16; ++i)
    {
      mDataReg[i].sData = 0;
      mDataReg[i].sIsWrite = false;
    }


  while( 1 )
    {
      flag == mCcu->CamacNAF( mStation , 0 , 4 );
      assert( 0 <= flag );

      if ( flag<0 )
	{
	  return -1;
	}
      if (  0 == mCcu->getCamacQ() )
	{
	  break;
	}
      else
	{

	  int tempdata = (int)(mCcu->getCamacData());
	  
	  channel = tempdata >> 12;	  
	  mDataReg[channel].sIsWrite = true;
	  mDataReg[channel].sData = tempdata & 0x0fff ;
	  ++n;
	  /*
	  cout << "Data of channel "
	       << channel+1 << " = "
	       << ( mCcu->getCamacData() & 0x0fff) << endl;
	  */
	  // add code of sending data to guests
	  /****************************************************/
      //SendData(  tempdata  );//???????????????????????????????
	  //  cout<<tempdata<<endl;
	  /****************************************************/


	}
    }
  return n;
}


bool CAdc::config( ADC_Config config_adc )
{
  clrHitReg_LAM_Data();

  for (int i = 0; i < 16 ; ++i)
    {
      setParaUT( i , config_adc.Para[i][0] );
      setParaLT( i , config_adc.Para[i][1] );
      setParaPED( i , config_adc.Para[i][2] );
    }
  
  setCtrlReg( config_adc.Ctrl );

  return true;

}


bool CAdc::slctPED()
{
  short flag;
  return setReg2( 0 );
}


bool CAdc::slctLT()
{
  short flag;
  return setReg2( 1 );
}


bool CAdc::slctUT()
{
  short flag;
  return setReg2( 2 );
}


bool CAdc::setParaReg(int a, long data)
{
  short flag;
  mCcu->setCamacData( data );
  assert( ( data>=-4095 ) && ( data<=4095 ) );

  flag = mCcu->CamacNAF( mStation , a , 20 );
  assert( flag>=0 );
  return true;
  
}

/*
bool CAdc::waitEvent(int timeout)
{
  assert( timeout>0 );

  //  cout << "waiting......" << endl;

  clock_t start, now;
  start = clock();
  while( 1 )
    {
      now = clock();

      if ( testLam() )
	{
	  return true;
	}
      if ( ( timeout / 1000 * CLOCKS_PER_SEC ) < ( now - start ) )
	{
	  //	  cout << "timeout......" << endl;
	  return false;
	}
    }
}
*/


short CAdc::getCtrlReg()
{
  short flag;
  flag = mCcu->CamacNAF( mStation , 0 , 6 );

  assert( flag>=0 );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return (short)mCcu->getCamacData();
    }
  
}


bool CAdc::setCtrlReg(short data)
{
  short flag;
  mCcu->setCamacData( data );
  assert( (data >=0) && (data<=7) );

  flag = mCcu->CamacNAF( mStation , 0 , 19 );
  assert( flag>=0 );
  return true;
    
}


bool CAdc::enableUT()
{
  short data;
  data = getCtrlReg() | 4 ;
  return setCtrlReg( data );
}


bool CAdc::disableUT()
{
  short data;
  data = getCtrlReg() & 3 ;
  return setCtrlReg( data );
}


bool CAdc::enableLT()
{
  short data;
  data = getCtrlReg() | 2 ;
  return setCtrlReg( data );
}


bool CAdc::disableLT()
{
  short data;
  data = getCtrlReg() & 5 ;
  return setCtrlReg( data );
}


bool CAdc::enablePED()
{
  short data;
  data = getCtrlReg() | 1 ;
  return setCtrlReg( data );
}


bool CAdc::disablePED()
{
  short data;
  data = getCtrlReg() & 6 ;
  return setCtrlReg( data );
}


unsigned short CAdc::getHitReg()
{
  unsigned short flag;
  flag = mCcu->CamacNAF( mStation , 1 , 6 );

  assert( flag>=0 );
  if ( flag < 0 )
    {
      return -1;
    }
  else
    {
      return (short)mCcu->getCamacData();
    }
  
}


bool CAdc::clrCtrlReg()
{
  return clrReg2(0);
}


bool CAdc::clrHitReg_LAM()
{
  return clrReg2(1);
}


bool CAdc::clrHitReg_LAM_Data()
{
  return clrReg2( 3 );
}


bool CAdc::getParaReg( int a, sParm* para)
{

  if( false == slctUT())
    return false;
  para->UT = getReg2( a );

  if( false == slctLT())
    return false;
  para->LT = getReg2( a );

  if( false == slctPED())
    return false;
  para->PED = getReg2( a );

  //assert(para->UT == 4095);
  //assert(para->LT == 50 );
  //assert(para->PED == 0 );

  return true;
}


bool CAdc::getParaRegAll(sParm* para)
{
  for (int i = 0; i < 16; ++i)
    {
      if ( false == getParaReg( i , para + i )  )
	{
	  return false;
	}
    }
  return true;
}


bool CAdc::setParaUT( int a, short data)
{
  slctUT();
  return setParaReg( a, data);
}


bool CAdc::setParaLT( int a, short data)
{
  slctLT();
  return setParaReg( a, data);
}


bool CAdc::setParaPED( int a, short data)
{
  slctPED();
  return setParaReg( a, data);
}


