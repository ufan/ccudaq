/***************************************************
# File Name:	display.h
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/
#include "global.h"
#include "curses.h"
#include <string>
#include "adc.h"

using namespace std;

class CDisplay
{
 public:
  CDisplay();
  virtual ~CDisplay();
 public:
  void output(string );
  void prompt();
  void form( CAdc*,bool );
  void form();
  int getCmd();

 private:
  sParm mPara[16];

  WINDOW* form_win;
  WINDOW* prompt_win;
  WINDOW* command_win;

};



