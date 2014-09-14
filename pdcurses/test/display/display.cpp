/***************************************************
# File Name:	display.cpp
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#include "display.h"

CDisplay::CDisplay()
{
  for (int i = 0; i < 16; ++i)
    {
      mPara[i].LT = 0;
      mPara[i].UT = 0;
      mPara[i].PED = 0;
    }

  initscr();
  resize_term(38,80);

  form_win = newwin(38,40,0,0);
  prompt_win = newwin(8,40,0,40);
  command_win = newwin(30,40,8,40);

  box(form_win,0,0);
  //box(prompt_win,0,0);
  //box(command_win,0,0);

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

  /*
      wscanw(out_win,str);
      scrollok(out_win,1);
      wprintw(out_win,"zzlucky");
      wrefresh(out_win);
  */

  wrefresh(command_win);

}


void CDisplay::form()
{
  // tittles
  wattron(form_win,A_REVERSE);
  mvwprintw(form_win , 1 , 1 ,
	     "         ADC Settings & Datas         " );
  wattroff(form_win,A_REVERSE);

  // context
  mvwprintw(form_win,3,2,"Chl\tUT\tLT\tPED\tData");


  for (int i = 0; i < 16; ++i)
    {
      mvwprintw(form_win, 2*i+5, 2,
		"%d\t%d\t%d\t%d\t%d",
		i+1 , 0 , 0 , 0 ,0 );
    }

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
  else if( 0 == str.compare("check") )
    return 2;
  else if( 0 == str.compare("go") )
    return 3;
  else if( 0 == str.compare("stop") )
    return 4;
  else if( 0 == str.compare("quit") )
    return 0;
  else
    return -1;

}


