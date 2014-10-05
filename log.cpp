/***************************************************
# File Name:	log.cpp
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#include "log.h"

using namespace std;

CLog::CLog( const char* str ,bool isfirst)
{
  logstr = str;
  if(isfirst){
        fp.open(fileChar.c_str());
  }
  else{
        fp.open( fileChar.c_str() , ios_base::app );
  }
  assert( fp );
}

CLog::CLog( string str ,bool isfirst)
{
  logstr = str;
  if(isfirst){
        fp.open(fileChar.c_str());
  }
  else{
        fp.open( fileChar.c_str() , ios_base::app );
  }
  assert( fp );
}

CLog::~CLog()
{

  fp << logstr <<endl;
  fp.close();
}

string CLog::fileChar;


