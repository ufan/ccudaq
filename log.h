/***************************************************
# File Name:	log.h
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#ifndef _LOG_H_
#define _LOG_H_

#include "global.h"
#include <fstream>
#include <string>

using namespace std;

class CLog
{
 public:
  CLog( const char* );
  virtual ~CLog();

 private:
  ofstream fp;
  string logstr;

 public:
  static string fileChar;
};


#endif /* _LOG_H_ */

