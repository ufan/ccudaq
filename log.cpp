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
  fp.open( fileChar , ios::app );
  assert( fp );
}


CLog::~CLog()
{

  fp << logstr <<endl;
  fp.close();
}

const char* CLog::fileChar;


