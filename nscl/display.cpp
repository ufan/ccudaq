/***************************************************
# File Name:	display.cpp
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#include "display.h"
#include "../pdcurses/include/curses.h"

std::string  CDisplay::PMT_prompt="pmt_$$";
std::string CDisplay::Normal_prompt=">>";

CDisplay::CDisplay():
    isPMT(false)
{
  initscr();
  //init color pairs
  {
          start_color();
          init_pair(1, COLOR_RED,     COLOR_BLACK);
          init_pair(2, COLOR_GREEN,   COLOR_BLACK);
          init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
          init_pair(4, COLOR_BLUE,    COLOR_BLACK);
          init_pair(5, COLOR_CYAN,    COLOR_BLACK);
          init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
          init_pair(7, COLOR_WHITE,   COLOR_BLACK);
 }
  //terminal sizes,
  //it is hard-coded now.may be better to be parametrized
  resize_term(60,80);
  status_win=newwin(20,40,0,0);
  form_win = newwin(40,40,20,0);
  prompt_win = newwin(20,40,0,40);
  command_win = newwin(40,40,20,40);

  box(form_win,0,0);

  //initial printing
  normal_status(true,NULL,NULL);
  form();
  prompt();
  output("Program Started.");

}

CDisplay::~CDisplay()
{
  endwin();
}

//filename: data saved into this file
//info: user-defined content, usually buffers transfered,event counts,waiting...
void CDisplay::normal_status(bool IsIdle,const char* filename,const char* info)
{
    wclear(status_win);
    //header
    wattron(status_win,A_REVERSE);
    mvwprintw(status_win,1,1,"      Status Information      ");
    wattroff(status_win,A_REVERSE);
    //mode
    if(IsIdle){
        wattron(status_win,COLOR_PAIR(4));
        mvwprintw(status_win,2,1,"Mode: normal\t\t\tStatus: idle");
        wattroff(status_win,COLOR_PAIR(4));
        //filename
        if(filename){
            //filename
            mvwprintw(status_win,4,1,"FileName: %s",filename);
            mvwprintw(status_win,6,1,"Info:");
            mvwprintw(status_win,7,1,"No DAQ Cycle Running.");
        }
        else{
            //other info
            mvwprintw(status_win,4,1,"Info:");
            mvwprintw(status_win,5,1,"No DAQ Cycle Running");
        }
    }
    else{
        wattron(status_win,COLOR_PAIR(4));
        mvwprintw(status_win,2,1,"Mode: normal\t\t\tStatus: DAQ Cycle");
        wattroff(status_win,COLOR_PAIR(4));
        //filename
        mvwprintw(status_win,4,1,"FileName: %s",filename);
        //other info
        mvwprintw(status_win,6,1,"Info:");
        mvwprintw(status_win,7,1,"%s",info);
    }

    wrefresh(status_win);
}

//pulser_status,hv_status: -1 can't connected,0 unconnected, 1 connected
//testDir: pmt testing dir
//output: user-defined output content, usually HV_Step, LED_config...
void CDisplay::pmt_status(bool IsIdle,int pulser_status,int hv_status,const char* testDir,const char* output)
{
    wclear(status_win);
   //header
    wattron(status_win,A_REVERSE);
    mvwprintw(status_win,1,1,"      Status Information      ");
    wattroff(status_win,A_REVERSE);

    if(IsIdle){
        //mode
        wattron(status_win,COLOR_PAIR(4));
        mvwprintw(status_win,2,1,"Mode: PMT\t\t\tStatus: Idle");
        wattroff(status_win,COLOR_PAIR(4));
        //pulser
        switch(pulser_status)
        {
        case 0:
        {
            mvwprintw(status_win,3,1,"LED Pulser: ");
            wattron(status_win,A_BLINK);
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK);
            wprintw(status_win,"\t\t");
            break;
        }
        case -1:
        {
            mvwprintw(status_win,3,1,"LED Pulser: ");
            wattron(status_win,A_BLINK | COLOR_PAIR(1));
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK | COLOR_PAIR(1));
            wprintw(status_win,"\t\t");
            break;
        }
        case 1:
        {
            mvwprintw(status_win,3,1,"LED Pulser: ");
            wattron(status_win,A_BLINK | COLOR_PAIR(2));
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK | COLOR_PAIR(2));
            wprintw(status_win,"\t\t");
            break;
        }
        }
        //hv
        switch(hv_status)
        {
        case 0:
        {
            wprintw(status_win,"SY1527: ");
            wattron(status_win,A_BLINK);
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK);
            //wprintw(status_win,"\t\t");
            break;
        }
        case -1:
        {
            wprintw(status_win,"SY1527: ");
            wattron(status_win,A_BLINK | COLOR_PAIR(1));
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK | COLOR_PAIR(1));
            //wprintw(status_win,"\t\t");
            break;
        }
        case 1:
        {
            wprintw(status_win,"SY1527: ");
            wattron(status_win,A_BLINK | COLOR_PAIR(2));
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK | COLOR_PAIR(2));
            //wprintw(status_win,"\t\t");
            break;
        }
        }
        //test dir
        mvwprintw(status_win,5,1,"Testing Dir:  NULL");
        //other info
        mvwprintw(status_win,7,1,"Info:");
        mvwprintw(status_win,8,1,"%s",output);
    }
    else{
        //mode
        wattron(status_win,COLOR_PAIR(4));
        mvwprintw(status_win,2,1,"Mode: PMT\t\t\tStatus: BUSY");
        wattroff(status_win,COLOR_PAIR(4));
        //pulser
        switch(pulser_status)
        {
        case 0:
        {
            mvwprintw(status_win,3,1,"LED Pulser: ");
            wattron(status_win,A_BLINK);
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK);
            wprintw(status_win,"\t\t");
            break;
        }
        case -1:
        {
            mvwprintw(status_win,3,1,"LED Pulser: ");
            wattron(status_win,A_BLINK | COLOR_PAIR(1));
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK | COLOR_PAIR(1));
            wprintw(status_win,"\t\t");
            break;
        }
        case 1:
        {
            mvwprintw(status_win,3,1,"LED Pulser: ");
            wattron(status_win,A_BLINK | COLOR_PAIR(2));
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK | COLOR_PAIR(2));
            wprintw(status_win,"\t\t");
            break;
        }
        }
        //hv
        switch(hv_status)
        {
        case 0:
        {
            wprintw(status_win,"SY1527: ");
            wattron(status_win,A_BLINK);
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK);
            //wprintw(status_win,"\t\t");
            break;
        }
        case -1:
        {
            wprintw(status_win,"SY1527: ");
            wattron(status_win,A_BLINK | COLOR_PAIR(1));
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK | COLOR_PAIR(1));
            //wprintw(status_win,"\t\t");
            break;
        }
        case 1:
        {
            wprintw(status_win,"SY1527: ");
            wattron(status_win,A_BLINK | COLOR_PAIR(2));
            waddch(status_win,ACS_BLOCK);
            wattron(status_win,A_BLINK | COLOR_PAIR(2));
            //wprintw(status_win,"\t\t");
            break;
        }
        }
        //test dir
        mvwprintw(status_win,5,1,"Testing Dir:  %s",testDir);
        //other info
        mvwprintw(status_win,7,1,"Info:");
        mvwprintw(status_win,8,1,"%s",output);
    }

    wrefresh(status_win);
}

void CDisplay::output( string str )
{
  str += "\n";
  scrollok(command_win,1);
  wprintw(command_win, str.c_str() );

  wrefresh(command_win);

}

void CDisplay::formSingleModule(Module_Config& config)
{
    wclear(form_win);
  // tittles
  wattron(form_win,A_REVERSE);
  mvwprintw(form_win , 1 , 1 ,
         "    Station: %-2d, Name: %-10s   ", config.getStation(),config.getName());
  wattroff(form_win,A_REVERSE);

  // context
  mvwprintw(form_win,3,2,"Chl\tUT\tLT\tPED\t");

  for (int i = 0; i < 16; ++i)
    {
      mvwprintw(form_win, 2*i+5, 2,
        "%-4d\t%-4d\t%-4d\t%-4d\t",
        i+1, config.getUT(i+1),config.getLT(i+1),config.getPED(i+1));
    }

  mvwprintw(form_win,37,2,"Ctrl\t%-4d\t%-4d\t%-4d",
            config.getCtrl()&0x4,config.getCtrl()&0x2,config.getCtrl()&0x1);

  wrefresh(form_win);
}

void CDisplay::formCCU(CC_Config &config_ccu, ModuleConfigFactory &config_modules)
{
    wclear(form_win);

    wattron(form_win,A_REVERSE);
    mvwprintw(form_win , 1 , 1 ,
           "     CCUSB & Module Configurations    ");
    wattroff(form_win,A_REVERSE);

    //CC-USB
    wattron(form_win,A_REVERSE);
    mvwprintw(form_win,3,2,"CC-USB register:");
    wattroff(form_win,A_REVERSE);
    mvwprintw(form_win,5,2,"GlobalMode\t\t\t0x%-8x",config_ccu.getGlobalMode());
    mvwprintw(form_win,7,2,"Delays\t\t\t0x%-8x",config_ccu.getDelays());
    mvwprintw(form_win,9,2,"ScalReadCtrl\t\t\t0x%-8x",config_ccu.getScalReadCtrl());
    mvwprintw(form_win,11,2,"SelectLED\t\t\t0x%-8x",config_ccu.getSelectLED());
    mvwprintw(form_win,13,2,"SelectNIM\t\t\t0x%-8x",config_ccu.getSelectNIMO());
    mvwprintw(form_win,15,2,"SelectUserDev\t\t\t0x%-8x",config_ccu.getSelectUserDevice());
    mvwprintw(form_win,17,2,"DGGA\t\t\t\t0x%-8x",config_ccu.getTimingDGGA());
    mvwprintw(form_win,19,2,"DGGB\t\t\t\t0x%-8x",config_ccu.getTimingDGGB());
    mvwprintw(form_win,21,2,"ExtDGG\t\t\t0x%-8x",config_ccu.getExtendedDelay());
    mvwprintw(form_win,23,2,"LAMMask\t\t\t0x%-8x",config_ccu.getLAMMask());
    mvwprintw(form_win,25,2,"UsbBufferSetup\t\t0x%-8x",config_ccu.getUsbBufferSetup());

    //Modules Summary
    int size=config_modules.size();
    wattron(form_win,A_REVERSE);
    mvwprintw(form_win,28,2,"%-2d Modules in this crate:",size);
    wattroff(form_win,A_REVERSE);
    string name;
    int station;
    for(int i=0;i<size;i++){
        name=config_modules[i]->getName();
        station=config_modules[i]->getStation();
        mvwprintw(form_win,30+i*2,2,"Station_%-2d: %-10s",station,name.c_str());
    }

    wrefresh(form_win);
}

void CDisplay::form()
{
  // tittles
  wattron(form_win,A_REVERSE);
  mvwprintw(form_win , 1 , 1 ,
         "     CCUSB & Module Configurations    ");
  wattroff(form_win,A_REVERSE);

  /*
  mvwprintw(form_win,3,2,"Chl\tUT\tLT\tPED\tData");


  for (int i = 0; i < 16; ++i)
    {
      mvwprintw(form_win, 2*i+5, 2,
        "%-4d\t%-4d\t%-4d\t%-4d\t%-4d",
		i+1 , 0 , 0 , 0 ,0 );
    }
*/
  wrefresh(form_win);
}

void CDisplay::prompt()
{
  wattron(prompt_win,A_REVERSE);
  mvwprintw( prompt_win , 1 , 1 ,
	     "               Command               " );
  wattroff(prompt_win,A_REVERSE);

  // config
  mvwprintw(prompt_win,3,1,"Type ");
  wattron(prompt_win,COLOR_PAIR(1));
  wattron(prompt_win,A_UNDERLINE);
  mvwprintw(prompt_win,3,6,"config");
  wattroff(prompt_win,COLOR_PAIR(1));
  wattroff(prompt_win,A_UNDERLINE);
  mvwprintw(prompt_win,3,12," to Config devices");

  // check
  mvwprintw(prompt_win,4,1,"Type ");
  wattron(prompt_win,COLOR_PAIR(1));
  wattron(prompt_win,A_UNDERLINE);
  mvwprintw(prompt_win,4,6,"check ");
  wattroff(prompt_win,COLOR_PAIR(1));
  wattroff(prompt_win,A_UNDERLINE);
  mvwprintw(prompt_win,4,12," to Check customers");

  // start
  mvwprintw(prompt_win,5,1,"Type ");
  wattron(prompt_win,COLOR_PAIR(1));
  wattron(prompt_win,A_UNDERLINE);
  mvwprintw(prompt_win,5,6,"start ");
  wattroff(prompt_win,COLOR_PAIR(1));
  wattroff(prompt_win,A_UNDERLINE);
  mvwprintw(prompt_win,5,12," to Start DAQ cycle");

  // stop
  mvwprintw(prompt_win,6,1,"Type ");
  wattron(prompt_win,COLOR_PAIR(1));
  wattron(prompt_win,A_UNDERLINE);
  mvwprintw(prompt_win,6,6,"stop  ");
  wattroff(prompt_win,COLOR_PAIR(1));
  wattroff(prompt_win,A_UNDERLINE);
  mvwprintw(prompt_win,6,12," to Stop DAQ cycle");

  // quit
  mvwprintw(prompt_win,7,1,"Type ");
  wattron(prompt_win,COLOR_PAIR(1));
  wattron(prompt_win,A_UNDERLINE);
  mvwprintw(prompt_win,7,6,"quit  ");
  wattroff(prompt_win,COLOR_PAIR(1));
  wattroff(prompt_win,A_UNDERLINE);
  mvwprintw(prompt_win,7,12," to Quit");

  wrefresh(prompt_win);
}

int CDisplay::getCmd()
{
    if(isPMT){
        wprintw(command_win,"%s ",PMT_prompt.c_str());
    }
    else{
        wprintw(command_win,"%s ",Normal_prompt.c_str());
    }

  char ch[100];

  wgetstr(command_win,ch);
  string str(ch);

  if( 0 == str.compare("config") ){
    return 1;
  }
  else if( 0 == str.compare("start") ){
    return 2;
  }
  else if( 0 == str.compare("stop") ){
    return 3;
  }
  else if( 0 == str.compare("open")){
      if(isPMT){
        return -1;
      }
      else{
        wprintw(command_win,"input: ");
        wgetstr(command_win,ch);
        filename=ch;
        return 4;
      }
  }
  else if( 0 == str.compare("show_ccu")){
    return 5;
  }
  else if( 0 == str.compare("show_module")){
    wprintw(command_win,"input: ");
    wgetstr(command_win,ch);
    module_name=ch;
    return 6;
  }
  else if( 0 == str.compare("quit") ){
    return 0;
  }
  else if(0 == str.compare("pmt")){
    if(isPMT){
        return -1;
    }
    else{
        isPMT=true;
        return 7;
    }
  }
  else if(0 == str.compare("exit")){
    if(isPMT){
        isPMT=false;
        return 8;
    }
    else{
        return -1;
    }
  }
  else if(0 == str.compare("mkdir")){
      if(isPMT){
        wprintw(command_win,"input: ");
        wgetstr(command_win,ch);
        PMTdir=ch;
        return 11;
      }
      else{
        return -1;
      }
  }
  else if(0 == str.compare("setdir")){
    if(isPMT){
        wprintw(command_win,"input: ");
        wgetstr(command_win,ch);
        PMTdir=ch;
        return 12;
    }
    else{
        return -1;
    }
  }
  else if(0 == str.compare("connect")){
    if(isPMT){
        return 13;
    }
    else{
        return -1;
    }
  }
  else{
    return -1;
  }

}

string CDisplay::getFilename()
{
    return filename;
}

string CDisplay::getPMTdir()
{
    return PMTdir;
}

string CDisplay::getModulename()
{
    return module_name;
}
