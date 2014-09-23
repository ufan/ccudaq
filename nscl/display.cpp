/***************************************************
# File Name:	display.cpp
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#include "display.h"

CDisplay::CDisplay()
{
  initscr();
  resize_term(38,80);

  form_win = newwin(40,40,0,0);
  prompt_win = newwin(8,40,0,40);
  command_win = newwin(32,40,8,40);

  box(form_win,0,0);

  form();
  prompt();
  output("Program Started.");

}

CDisplay::~CDisplay()
{
  endwin();
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
    mvwprintw(form_win,17,2,"DGGA\t\t\t0x%-8x",config_ccu.getTimingDGGA());
    mvwprintw(form_win,19,2,"DGGB\t\t\t0x%-8x",config_ccu.getTimingDGGB());
    mvwprintw(form_win,21,2,"ExtDGG\t\t\t0x%-8x",config_ccu.getExtendedDelay());
    mvwprintw(form_win,23,2,"LAMMask\t\t\t0x%-8x",config_ccu.getLAMMask());
    mvwprintw(form_win,25,2,"UsbBufferSetup\t\t\t0x%-8x",config_ccu.getUsbBufferSetup());

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

  start_color();
  init_pair(1,COLOR_RED,COLOR_BLACK);

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
  mvwprintw(prompt_win,5,6,"go ");
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
  wprintw(command_win,">> ");
  char ch[100];

  wgetstr(command_win,ch);
  string str(ch);

  if( 0 == str.compare("config") )
    return 1;
  else if( 0 == str.compare("go") )
    return 2;
  else if( 0 == str.compare("stop") )
    return 3;
  else if( 0 == str.compare("open")){
      wprintw(command_win,"input: ");
      wgetstr(command_win,ch);
      filename=ch;
      return 4;
  }
  else if( 0 == str.compare("quit") )
    return 0;
  else
    return -1;
}

string CDisplay::getFilename()
{
    return filename;
}

