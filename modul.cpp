/***************************************************
# File Name:	modul.cpp
# Abstract:	
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#include "modul.h"

#include <iostream>
using namespace std;

CModul::CModul( CCcusb* ccusb , int n )
{
  mCcu = ccusb;
  mStation = n;

  /*
  cout << "Modul on station "
       << mStation
       << " Signed. " << endl;
  */
}


bool CModul::enableLam()
{
  short flag;
  flag = mCcu->CamacNAF( mStation , 0 , 26 );

  assert( flag>=0 );
  return true;

}


bool CModul::disableLam()
{
  short flag;
  flag = mCcu->CamacNAF( mStation , 0 , 24 );

  assert( flag>=0 );
  return true;
  
}


bool CModul::testLam()
{
  short flag;
  flag = mCcu->CamacNAF( mStation , 0 , 8 );

  assert( flag>=0 );

  if ( 0 == mCcu->getCamacQ() )
    {
      return false;
    }
  else
    {
      return true;
    }

}


bool CModul::clrLam()
{
  short flag;
  flag = mCcu->CamacNAF( mStation , 0 , 10 );

  assert( flag>=0 );
  return true;

}


long CModul::getReg1(int a)
{
  short flag;
  flag = mCcu->CamacNAF( mStation , a , 0 );

  assert( flag>=0 );
  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return mCcu->getCamacData();
    }
  
}


bool CModul::setReg1(int a, long data)
{
  short flag;
  mCcu->setCamacData( data );
  flag = mCcu->CamacNAF( mStation , a , 16 );

  assert( flag>=0 );
  return true;
  
}


bool CModul::clrReg1(int a)
{
  short flag;
  flag = mCcu->CamacNAF( mStation , a , 9 );

  assert( flag>=0 );
  return true;

}


long CModul::getReg2(int a)
{
  short flag;
  flag = mCcu->CamacNAF( mStation , a , 1 );

  assert( flag>=0 );

  if ( flag<0 )
    {
      return -1;
    }
  else
    {
      return mCcu->getCamacData();
    }
  
}


bool CModul::setReg2(int a, long data )
{
  short flag;
  mCcu->setCamacData( data );
  flag = mCcu->CamacNAF( mStation , a , 17 );

  assert( flag>=0 );
  return true;
  
}


bool CModul::clrReg2(int a)
{
  short flag;
  flag = mCcu->CamacNAF( mStation , a , 11 );

  assert( flag>=0 );
  return true;

}


bool CModul::getReg1All(long* data)
{
  short flag;
  for (int i = 0; i < 16; ++i)
    {
      flag = mCcu->CamacNAF( mStation , i , 0 );

      assert( flag>=0 );

      *(data+i) = mCcu->getCamacData();
    }

  return true;

}


bool CModul::getReg2All(long* data)
{
  short flag;
  for (int i = 0; i < 16; ++i)
    {
      flag = mCcu->CamacNAF( mStation , i , 1 );

      if ( flag<0 )
	{
	  return NULL;
	}
      *(data+i) = mCcu->getCamacData();
    }
  return data;  
  
}

/*
bool CModul::setRegAuto()
{
  return true; 
}
*/


bool CModul::clrReg1All()
{
  short flag;
  for (int i = 0; i < 16; ++i)
    {
      flag = mCcu->CamacNAF( mStation , i , 9 );
      assert( flag>=0 );
    }
  return true;

}


bool CModul::clrReg2All()
{
  short flag;
  for (int i = 0; i < 16; ++i)
    {
      flag = mCcu->CamacNAF( mStation , i ,11 );
      assert( flag>=0 );

    }
  return true;

}

