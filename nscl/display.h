/***************************************************
# File Name:	display.h
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/
#include "config.h"
#include "curses.h"
#include <string>

using namespace std;

class CDisplay
{
 public:
  CDisplay();
  virtual ~CDisplay();
 public:
  void output(string );
  void prompt();
  void formSingleModule(Module_Config& config);
  void formCCU(CC_Config& config_ccu,ModuleConfigFactory& config_modules);
  void form();
  int getCmd();
  string getFilename();

 private:
  string filename;
  string PMTdir;

  WINDOW* form_win;
  WINDOW* prompt_win;
  WINDOW* command_win;

};



