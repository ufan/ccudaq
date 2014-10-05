/***************************************************
# File Name:	log.h
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#ifndef _LOG_H_
#define _LOG_H_

#define NDEBUG
#include <assert.h>
#include <fstream>
#include <string>

class CLog
{
 public:
  CLog( const char* str,bool isfirst=false);
  CLog(std::string str,bool isfirst=false);
  virtual ~CLog();

 private:
  std::ofstream fp;
  std::string logstr;

 public:
  static std::string fileChar;
};


#endif /* _LOG_H_ */

