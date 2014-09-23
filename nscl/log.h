/***************************************************
# File Name:	log.h
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#ifndef _LOG_H_
#define _LOG_H_

#include <fstream>
#include <string>

using namespace std;

class CLog
{
 public:
  CLog( const char* str,bool isfirst=false);
  CLog(string str,bool isfirst=false);
  virtual ~CLog();

 private:
  ofstream fp;
  string logstr;

 public:
  static string fileChar;
};


#endif /* _LOG_H_ */

