/***************************************************
# File Name:	log.cpp
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#include "log.h"

CLog::CLog( const char* str )
{
  logstr = str;
  fp.open( fileChar.c_str() , ios::app );
  assert( fp );
}


CLog::~CLog()
{

  fp << logstr <<endl;
  fp.close();
}

string CLog::fileChar;


