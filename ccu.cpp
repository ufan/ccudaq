/***************************************************
# File Name:	ccu.cpp
# Abstract:	Including functions about visiting CC-USB
# Author:	zhangzhelucky
# Update History:
Thu Apr 11 14:57:15 2013  Implemented most of the functions
****************************************************/

#include "ccu.h"

#include <iostream>
using namespace std;

CCcusb::CCcusb(xxusb_device_type lpUsbDev)
{
  mSerialStr = string(lpUsbDev.SerialString);
  DeviceOpen(lpUsbDev);
}


CCcusb::~CCcusb()
{
  DeviceClose();
}


bool CCcusb::DeviceOpen(xxusb_device_type lpUsbDev)
{
  mDevHandle = xxusb_device_open(lpUsbDev.usbdev);

  if (0 == mDevHandle)
    {
      return false;
    }
  else
    {
      cout << "Device "<< mSerialStr <<" Open" << endl;
      return true;
    }

}


bool CCcusb::DeviceClose()
{
  short flag;
  flag = xxusb_device_close( mDevHandle );

  if ( flag<0 )
    {
      return false;
    }
  else
    {
      cout << "Device "<< mSerialStr <<" Closed. " << endl;
      return true;
    }
}


bool CCcusb::Config( CC_Config config_cc )
{
  setGlobalMode( config_cc.GlobalMode );
  setDelays( config_cc.Delays );
  setScalReadCtrl( config_cc.ScalReadCtrl );
  setSelectLED( config_cc.SelectLED );
  setSelectNIMO( config_cc.SelectNIMO );
  setSelectUserDevice( config_cc.SelectUserDevice );
  setTimingDGGA( config_cc.TimingDGGA );
  setTimingDGGB( config_cc.TimingDGGB );
  setLAMMask( config_cc.LAMMask );
  setExtendedDelay( config_cc.ExtendedDelay );
  setUsbBufferSetup( config_cc.UsbBufferSetup );

  return true;
}


short CCcusb::CamacNAF(int N,int A,int F)
{
  mCamacQ = 0;
  mCamacX = 0;

  short flag;
  assert( (N>=0) && (N<=31) && 
	  (A>=0) && (A<=15) && 
	  (F>=0) && (F<=31) );

  if ( F<16 || F>23 )
    {
      mCamacData = 0;
      flag = CAMAC_read( mDevHandle , N , A , F , 
			 &mCamacData , 
			 &mCamacQ , &mCamacX );
    }
  else
    {
      flag = CAMAC_write( mDevHandle ,N , A , F , 
			  mCamacData , 
			  &mCamacQ , &mCamacX );
      mCamacData = 0;
    }

  //  assert( 0 != mCamacX);

  return flag;

}


bool CCcusb::CamacZ()
{
  short flag;
  flag = CAMAC_Z( mDevHandle );

  assert( 0 <= flag );
  return true;      

}


bool CCcusb::CamacC()
{
  short flag;
  flag = CAMAC_C( mDevHandle );

  assert( 0 <= flag );
  return true;      

}


bool CCcusb::CamacI( bool inhi = true )
{
  short flag;
  if ( true==inhi )
    {
      flag = CAMAC_I( mDevHandle , 1 );      
    }
  else
    {
      flag = CAMAC_I( mDevHandle , 0 );
    }

  assert( 0 <= flag );
  return true;      
}


int CCcusb::getCamacQ()
{
  return mCamacQ;
}

int CCcusb::getCamacX()
{
  return mCamacX;
}

long CCcusb::getCamacData()
{
  return mCamacData;
}

bool CCcusb::setCamacData(long data)
{
  assert( data <=  0xffffff );

  mCamacData = data;
  return true;

}


long CCcusb::getFirmwareId()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0x0 , &data);

  assert( 0 <= flag );

  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getGlobalMode()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0x1 , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getDelays()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0x2 , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getScalReadCtrl()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0x3 , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}
long CCcusb::getSelectLED()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0x4 , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getSelectNIMO()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0x5 , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getSelectUserDevice()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0x6 , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getTimingDGGA()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0x7 , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getTimingDGGB()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0x8 , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getLAMMask()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0x9 , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getLAM()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0xa , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getScalerA()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0xb , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getScalerB()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0xc , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getExtendedDelay()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0xd , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getUsbBufferSetup()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0xe , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


long CCcusb::getBroadcastMap()
{
  long data;
  short flag;
  flag = CAMAC_register_read( mDevHandle , 0xf , &data);

  assert( 0 <= flag );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return data;
    }

}


bool CCcusb::setGlobalMode( long data )
{
  assert( data<= 0xffffff );

  short flag;
  flag = CAMAC_register_write( mDevHandle , 0x1 , data);

  assert( 0 <= flag );
  return true;

}


bool CCcusb::setDelays( long data )
{
  assert( data<= 0xffffff );

  short flag;
  flag = CAMAC_register_write( mDevHandle , 0x2 , data);
  assert( 0 <= flag );
  return true;

}

bool CCcusb::setScalReadCtrl( long data )
{
  assert( data<= 0xffffff );

  short flag;
  flag = CAMAC_register_write( mDevHandle , 0x3 , data);

  assert( 0 <= flag );
  return true;

}


bool CCcusb::setSelectLED( long data )
{

  short flag;
  flag = CAMAC_register_write( mDevHandle , 0x4 , data);

  assert( 0 <= flag );
  return true;

}


bool CCcusb::setSelectNIMO( long data )
{
  short flag;
  flag = CAMAC_register_write( mDevHandle , 0x5 , data);

  assert( 0 <= flag );
  return true;

}


bool CCcusb::setSelectUserDevice( long data )
{
  short flag;
  flag = CAMAC_register_write( mDevHandle , 0x06 , data);

  assert( 0 <= flag );
  return true;

}


bool CCcusb::setTimingDGGA( long data )
{
  short flag;
  flag = CAMAC_register_write( mDevHandle , 0x7 , data);

  assert( 0 <= flag );
  return true;

}


bool CCcusb::setTimingDGGB( long data )
{
  short flag;
  flag = CAMAC_register_write( mDevHandle , 0x8 , data);

  assert( 0 <= flag );
  return true;

}


bool CCcusb::setLAMMask( long data )
{
  assert( data<= 0xffffff );

  short flag;
  flag = CAMAC_register_write( mDevHandle , 0x9 , data);

  assert( 0 <= flag );
  return true;

}


bool CCcusb::setExtendedDelay( long data )
{
  short flag;
  flag = CAMAC_register_write( mDevHandle , 0xd , data);

  assert( 0 <= flag );
  return true;

}


bool CCcusb::setUsbBufferSetup( long data )
{
  short flag;
  flag = CAMAC_register_write( mDevHandle , 0xe , data);

  assert( 0 <= flag );
  return true;

}


