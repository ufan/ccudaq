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
  void normal_status(bool IsIdle,char* filename,char* info);
  void pmt_status(bool IsIdle,int pulser_status,int hv_status,char* testDir,char* output);
  void prompt();
  void formSingleModule(Module_Config& config);
  void formCCU(CC_Config& config_ccu,ModuleConfigFactory& config_modules);
  void form();
  int getCmd();
  string getFilename();

 private:
  string filename;
  string PMTdir;
  static string PMT_prompt;
  static string Normal_prompt;

  bool isPMT;

  WINDOW* form_win;
  WINDOW* status_win;
  WINDOW* prompt_win;
  WINDOW* command_win;

};



