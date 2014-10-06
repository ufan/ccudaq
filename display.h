/***************************************************
# File Name:	display.h
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/
#include "config.h"
#include "SYX527.h"
#include "curses.h"
#include <string>

class CDisplay
{
 public:
  CDisplay();
  virtual ~CDisplay();
 public:
  void output(std::string );
  void normal_status(bool IsIdle,const char* curdir,const char* filename,const char* info);
  void pmt_status(bool IsIdle,int pulser_status,int hv_status,const char* testDir,const char* output);
  void scroll_status(const char* msg);
  void prompt();
  void formPMT();
  void formSingleModule(Module_Config& config);
  void formCCU(CC_Config& config_ccu,ModuleConfigFactory& config_modules);
  void formHV(HVGroup& config_hv);
  void formatChannel(int row,HVChannel& channel);
  void form();
  int getCmd();
  std::string getFilename();
  std::string getPMTdir();
  std::string getCurrentDir();
  std::string getDirname();
  std::string getModulename();

 private:
  std::string filename;
  std::string CurrentDir;
  std::string dirname;
  std::string PMTdir;
  std::string module_name;
  static std::string PMT_prompt;
  static std::string Normal_prompt;

  bool isPMT;


  int term_x;
  int term_y;
  int form_x;
  int form_y;
  int status_x;
  int status_y;
  int prompt_x;
  int prompt_y;
  int command_x;
  int command_y;
  int sstatus_x;
  int sstatus_y;

  attr_t sstatus_attr;

  WINDOW* form_win;
  WINDOW* status_win;
  WINDOW* prompt_win;
  WINDOW* command_win;
  WINDOW* sstatus_win;
};



