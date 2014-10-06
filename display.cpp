/***************************************************
# File Name:	display.cpp
# Abstract:	 
# Author:	zhangzhelucky
# Update History:
	
****************************************************/

#include "display.h"
#include "curses.h"
#include <string.h>

using namespace std;
string  CDisplay::PMT_prompt="PMT_$$";
string CDisplay::Normal_prompt=">>";

CDisplay::CDisplay():
    isPMT(false),CurrentDir("./")
{
  initscr();

  //init color pairs
  start_color();
  init_pair(1, COLOR_RED,     COLOR_BLACK);
  init_pair(2, COLOR_GREEN,   COLOR_BLACK);
  init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
  init_pair(4, COLOR_BLUE,    COLOR_BLACK);
  init_pair(5, COLOR_CYAN,    COLOR_BLACK);
  init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(7, COLOR_WHITE,   COLOR_BLACK);

  //terminal sizes,
  term_y=52;term_x=155;
  resize_term(term_y,term_x);
  /******************************
   * status     *    prompt     *
   **************               *
   * sstatus    *               *
   ******************************
   *  form      *    command    *
   *            *               *
   ******************************/
  status_y=17;
  sstatus_y=5;
  status_x=55;
  sstatus_x=status_x;
  status_win=newwin(status_y-sstatus_y,status_x,0,0);
  sstatus_win = newwin(sstatus_y,sstatus_x,status_y-sstatus_y,0);

  form_y=term_y-status_y;
  form_x=status_x;
  form_win = newwin(form_y,form_x,status_y,0);

  prompt_y=status_y;
  prompt_x=term_x-status_x;
  prompt_win = newwin(prompt_y,prompt_x,0,status_x);

  command_y=form_y;
  command_x=prompt_x;
  command_win = newwin(command_y,command_x,status_y,status_x);
  //
  //box(form_win,0,0);
  //
  scrollok(command_win,true);
  scrollok(sstatus_win,true);
  //
  sstatus_attr=COLOR_PAIR(3);
  wattron(sstatus_win,sstatus_attr);
  //initial printing
  normal_status(true,"./",NULL,"Init State");
  scroll_status("Scroll Area");
  form();
  /*
  HVGroup config;
  HVChannels channels;
  HVChannel tempch;
  for(int i=0;i<5;i++){
      tempch.ch_id=i;
      sprintf(tempch.ch_name,"PMT%d",i+1);
      tempch.slot=13;
      tempch.V0Set=1200+i;
      tempch.I0Set=200+i;
      tempch.VMon=1150+i;
      tempch.IMon=45+i;
      tempch.state=true;

      channels.push_back(tempch);
  }
  config[13]=channels;
  formHV(config);
  */
  prompt();
  output("Program Started.");

}

CDisplay::~CDisplay()
{
  endwin();
}

//filename: data saved into this file
//info: user-defined content, usually buffers transfered,event counts,waiting...
void CDisplay::normal_status(bool IsIdle,const char* curdir,const char* filename,const char* info)
{
    wclear(status_win);
    //header
    char header[256]="Status Information";
    wattron(status_win,A_REVERSE);
    int pos=1;
    int temp=(status_x-strlen(header))/2-1;
    while(pos<temp){
        mvwaddch(status_win,1,pos,' ');
        pos++;
    }
    mvwprintw(status_win,1,temp,"%s",header);
    pos=temp+strlen(header);
    while(pos<status_x){
        mvwaddch(status_win,1,pos,' ');
        pos++;
    }
    wattroff(status_win,A_REVERSE);
    //mode
    if(IsIdle){
        wattron(status_win,COLOR_PAIR(5));
        mvwprintw(status_win,2,1,"Mode:  normal\t\tStatus:  idle");
        wattroff(status_win,COLOR_PAIR(5));
    }
    else{
        wattron(status_win,COLOR_PAIR(3));
        mvwprintw(status_win,2,1,"Mode:  normal\t\tStatus:  DAQ cycle");
        wattroff(status_win,COLOR_PAIR(3));
    }
    //filename
    if(filename==NULL || strlen(filename)==0){
        mvwprintw(status_win,4,1,"CurrentDir: %s",curdir);
        mvwprintw(status_win,6,1,"FileName: NOT SET");
    }
    else{
        mvwprintw(status_win,4,1,"CurrentDir: %s",curdir);
        mvwprintw(status_win,6,1,"FileName: %s",filename);
    }
    mvwprintw(status_win,8,1,"Info:");
    mvwprintw(status_win,9,1,"%s",info);

    wrefresh(status_win);
}

//pulser_status,hv_status: -1 can't connected,0 unconnected, 1 connected
//testDir: pmt testing dir
//output: user-defined output content, usually HV_Step, LED_config...
void CDisplay::pmt_status(bool IsIdle,int pulser_status,int hv_status,const char* testDir,const char* output)
{
    wclear(status_win);
   //header
    char header[256]="Status Information";
    wattron(status_win,A_REVERSE);
    int pos=1;
    int temp=(status_x-strlen(header))/2-1;
    while(pos<temp){
        mvwaddch(status_win,1,pos,' ');
        pos++;
    }
    mvwprintw(status_win,1,temp,"%s",header);
    pos=temp+strlen(header);
    while(pos<status_x){
        mvwaddch(status_win,1,pos,' ');
        pos++;
    }
    wattroff(status_win,A_REVERSE);
    //mode
    wattron(status_win,COLOR_PAIR(3));
    if(IsIdle){
        mvwprintw(status_win,2,1,"Mode: PMT\t\tStatus: Idle");
    }
    wattroff(status_win,COLOR_PAIR(3));
    //pulser
    switch(pulser_status)
    {
    case 0:
    {
        mvwprintw(status_win,4,1,"LED Pulser: ");
        wattron(status_win,A_BOLD);
        waddch(status_win,ACS_BLOCK);
        waddch(status_win,ACS_BLOCK);
        wattroff(status_win,A_BOLD);
        wprintw(status_win,"\t\t");
        break;
    }
    case -1:
    {
        mvwprintw(status_win,4,1,"LED Pulser: ");
        wattron(status_win,A_BOLD | COLOR_PAIR(1));
        waddch(status_win,ACS_BLOCK);
        waddch(status_win,ACS_BLOCK);
        wattroff(status_win,A_BOLD | COLOR_PAIR(1));
        wprintw(status_win,"\t\t");
        break;
    }
    case 1:
    {
        mvwprintw(status_win,4,1,"LED Pulser: ");
        wattron(status_win,A_BOLD | COLOR_PAIR(2));
        waddch(status_win,ACS_BLOCK);
        waddch(status_win,ACS_BLOCK);
        wattroff(status_win,A_BOLD | COLOR_PAIR(2));
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
        wattron(status_win,A_BOLD);
        waddch(status_win,ACS_BLOCK);
        waddch(status_win,ACS_BLOCK);
        wattroff(status_win,A_BOLD);
        break;
    }
    case -1:
    {
        wprintw(status_win,"SY1527: ");
        wattron(status_win,A_BOLD | COLOR_PAIR(1));
        waddch(status_win,ACS_BLOCK);
        waddch(status_win,ACS_BLOCK);
        wattroff(status_win,A_BOLD | COLOR_PAIR(1));
        break;
    }
    case 1:
    {
        wprintw(status_win,"SY1527: ");
        wattron(status_win,A_BOLD | COLOR_PAIR(2));
        waddch(status_win,ACS_BLOCK);
        waddch(status_win,ACS_BLOCK);
        wattroff(status_win,A_BOLD | COLOR_PAIR(2));
        break;
    }
    }
    //test dir
    mvwprintw(status_win,6,1,"Testing Dir:  %s",testDir);
    //other info
    mvwprintw(status_win,8,1,"Info:");
    mvwprintw(status_win,9,1,"%s",output);
    //
    wrefresh(status_win);
}

void CDisplay::scroll_status(const char *msg)
{
    wprintw(sstatus_win," %s",msg);
    waddch(sstatus_win,'\n');
/*
    for(int i=0;i<6;i++){
      wprintw(sstatus_win," %s\n",msg);
    }
    */
    wrefresh(sstatus_win);
}

void CDisplay::output( string str,MSGFUCK level )
{
  attr_t ATTR;
  switch (level) {
  case WARNING_T:
      ATTR=COLOR_PAIR(3);
      break;
  case ERROR_T:
      ATTR=COLOR_PAIR(1);
      break;
  case NORMAL_T:
      ATTR=COLOR_PAIR(7);
      break;
  case PMT_T:
      ATTR=COLOR_PAIR(5);
      break;
  default:
      break;
  }
  //
  wattron(command_win,ATTR);
  str += "\n";
  waddch(command_win,' ');
  wprintw(command_win, str.c_str() );
  wattroff(command_win,ATTR);

  wrefresh(command_win);

}

void CDisplay::formPMT(string str)
{
    wclear(form_win);
    //header
     char header[256]="PMT-testing Configuration";
     wattron(form_win,A_REVERSE);
     int pos=1;
     int temp=(form_x-strlen(header))/2-1;
     while(pos<temp){
         mvwaddch(form_win,1,pos,' ');
         pos++;
     }
     mvwprintw(form_win,1,temp,"%s",header);
     pos=temp+strlen(header);
     while(pos<form_x){
         mvwaddch(form_win,1,pos,' ');
         pos++;
     }
     wattroff(form_win,A_REVERSE);
     //
     mvwprintw(form_win,3,1,str.c_str());
     //
     wrefresh(form_win);
}

void CDisplay::formSingleModule(Module_Config& config)
{
    wclear(form_win);
  // tittles
    char header[256];
    sprintf(header,"Station: %-2d, Name: %-10s",config.getStation(),config.getName());
    wattron(form_win,A_REVERSE);
    int pos=1;
    int temp=(form_x-strlen(header))/2-1;
    while(pos<temp){
        mvwaddch(form_win,1,pos,' ');
        pos++;
    }
    mvwprintw(form_win,1,temp,"%s",header);
    pos=temp+strlen(header);
    while(pos<form_x){
        mvwaddch(form_win,1,pos,' ');
        pos++;
    }
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
    //
    //header
     char header[256]="CCUSB & Module Configurations";
     wattron(form_win,A_REVERSE);
     int pos=1;
     int temp=(form_x-strlen(header))/2-1;
     while(pos<temp){
         mvwaddch(form_win,1,pos,' ');
         pos++;
     }
     mvwprintw(form_win,1,temp,"%s",header);
     pos=temp+strlen(header);
     while(pos<form_x){
         mvwaddch(form_win,1,pos,' ');
         pos++;
     }
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

void CDisplay::formHV(HVGroup &config_hv)
{
    wclear(form_win);
    //title
     char header[256]="High Voltage Feedback";
     wattron(form_win,A_REVERSE);
     int pos=1;
     int temp=(form_x-strlen(header))/2-1;
     while(pos<temp){
         mvwaddch(form_win,1,pos,' ');
         pos++;
     }
     mvwprintw(form_win,1,temp,"%s",header);
     pos=temp+strlen(header);
     while(pos<form_x){
         mvwaddch(form_win,1,pos,' ');
         pos++;
     }
     wattroff(form_win,A_REVERSE);

    //colume header
    wattron(form_win,COLOR_PAIR(5)|A_REVERSE);
    mvwprintw(form_win,3,1,"Name\tCH\tVSet\tISet\tVMon\tIMon\tState");
    wattroff(form_win,COLOR_PAIR(5)|A_REVERSE);
     //columes
    HVGroup::iterator it;
    HVChannels tempChs;
    size_t channels_size;
    int row_num=3;

    for(it=config_hv.begin();it!=config_hv.end();it++){
        tempChs=it->second;
        channels_size=tempChs.size();
        for(size_t j=0;j<channels_size;j++){
            formatChannel(++row_num,tempChs[j]);
        }
    }
    //
    wrefresh(form_win);
}

void CDisplay::formatChannel(int row, HVChannel &channel)
{
    attr_t ATTR;
    if(row%2){
        ATTR=COLOR_PAIR(6);
    }
    else{
        ATTR=COLOR_PAIR(7);
    }

    wattron(form_win,ATTR);
    mvwprintw(form_win,row,1,"%s\t%d_%d\t%-7.1f\t%-7.1f\t%-7.1f\t%-7.1f\t",
              channel.ch_name,channel.slot,channel.ch_id,channel.V0Set,
              channel.I0Set,channel.VMon,channel.IMon);
    if(channel.state){
        wprintw(form_win,"On");
    }
    else{
        wprintw(form_win,"Off");
    }
    wattroff(form_win,ATTR);
}

void CDisplay::form()
{
  // tittles
    char header[256]="CCUSB & Module Configurations";
    wattron(form_win,A_REVERSE);
    int pos=1;
    int temp=(form_x-strlen(header))/2-1;
    while(pos<temp){
        mvwaddch(form_win,1,pos,' ');
        pos++;
    }
    mvwprintw(form_win,1,temp,"%s",header);
    pos=temp+strlen(header);
    while(pos<form_x){
        mvwaddch(form_win,1,pos,' ');
        pos++;
    }
    wattroff(form_win,A_REVERSE);

    wrefresh(form_win);
}

void CDisplay::prompt()
{
    //header
    char header[256]="Command Description";
    wattron(prompt_win,A_REVERSE);
    int pos=1;
    int temp=(prompt_x-strlen(header))/2-1;
    while(pos<temp){
        mvwaddch(prompt_win,1,pos,' ');
        pos++;
    }
    mvwprintw(prompt_win,1,temp,"%s",header);
    pos=temp+strlen(header);
    while(pos<prompt_x){
        mvwaddch(prompt_win,1,pos,' ');
        pos++;
    }
    wattroff(prompt_win,A_REVERSE);

    //sub_title
    char* sub_header;
    int common_pos,normal_pos,pmt_pos;
    int space=(prompt_x-6)/3;

    common_pos=1;
    sub_header=const_cast<char*>("COMMON");
    wattron(prompt_win,COLOR_PAIR(5)|A_REVERSE);
    mvwprintw(prompt_win,2,common_pos+(space-strlen(sub_header))/2,"%s",sub_header);
    wattroff(prompt_win,COLOR_PAIR(5)|A_REVERSE);

    normal_pos=common_pos+space+2;
    sub_header=const_cast<char*>("NORMAL");
    wattron(prompt_win,COLOR_PAIR(5)|A_REVERSE);
    mvwprintw(prompt_win,2,normal_pos+(space-strlen(sub_header))/2,"%s",sub_header);
    wattroff(prompt_win,COLOR_PAIR(5)|A_REVERSE);

    pmt_pos=normal_pos+space+2;
    sub_header=const_cast<char*>("PMT");
    wattron(prompt_win,COLOR_PAIR(5)|A_REVERSE);
    mvwprintw(prompt_win,2,pmt_pos+(space-strlen(sub_header))/2,"%s",sub_header);
    wattroff(prompt_win,COLOR_PAIR(5)|A_REVERSE);

    //
    char* command;
    char* description;
    attr_t command_attr=COLOR_PAIR(5)|A_BOLD;
    attr_t description_attr=COLOR_PAIR(3);

    //common command
    int start_row=3;

    start_row++;
    command=const_cast<char*>("quit");
    description=const_cast<char*>(":close the program");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,common_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,common_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("start");
    description=const_cast<char*>(":start daq cycle");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,common_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,common_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("stop");
    description=const_cast<char*>(":stop daq cycle");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,common_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,common_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("config");
    description=const_cast<char*>(":config daq system");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,common_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,common_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("show_ccu");
    description=const_cast<char*>(":CC-USB's config");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,common_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,common_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("show_module");
    description=const_cast<char*>(":a module's config");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,common_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,common_pos+strlen(command)+1,"%s",description);
    start_row++;
    description=const_cast<char*>(" need module's name");
    mvwprintw(prompt_win,start_row,common_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("mkdir");
    description=const_cast<char*>(":make a new directory");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,common_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,common_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    //normal mode command
    start_row=3;

    start_row++;
    command=const_cast<char*>("pmt");
    description=const_cast<char*>(":change to PMT mode");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,normal_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,normal_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("open");
    description=const_cast<char*>(":save data to file");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,normal_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,normal_pos+strlen(command)+1,"%s",description);
    start_row++;
    description=const_cast<char*>(" Need new filename");
    mvwprintw(prompt_win,start_row,normal_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("curdir");
    description=const_cast<char*>(":new output dir");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,normal_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,normal_pos+strlen(command)+1,"%s",description);
    start_row++;
    description=const_cast<char*>(" Need dir's name");
    mvwprintw(prompt_win,start_row,normal_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);
    //pmt mode command
    start_row=3;

    start_row++;
    command=const_cast<char*>("exit");
    description=const_cast<char*>(":change to Normal mode");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,pmt_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,pmt_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("config_pmt");
    description=const_cast<char*>(":config PMT testing");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,pmt_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,pmt_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("connect");
    description=const_cast<char*>(":check HV and Pulser");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,pmt_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,pmt_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("setdir");
    description=const_cast<char*>(":set testing dir");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,pmt_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,pmt_pos+strlen(command)+1,"%s",description);
    start_row++;
    description=const_cast<char*>(" Need existing dir's name");
    mvwprintw(prompt_win,start_row,pmt_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);

    start_row++;
    command=const_cast<char*>("show_pmt");
    description=const_cast<char*>(":PMT-tesing summary");
    wattron(prompt_win,command_attr);
    mvwprintw(prompt_win,start_row,pmt_pos,"%s",command);
    wattroff(prompt_win,command_attr);
    wattron(prompt_win,description_attr);
    mvwprintw(prompt_win,start_row,pmt_pos+strlen(command)+1,"%s",description);
    wattroff(prompt_win,description_attr);
    //
    wrefresh(prompt_win);
}

int CDisplay::getCmd()
{
    if(isPMT){
        wprintw(command_win," %s ",PMT_prompt.c_str());
    }
    else{
        wprintw(command_win," %s ",Normal_prompt.c_str());
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
        wprintw(command_win," input: ");
        wgetstr(command_win,ch);
        filename=ch;
        return 4;
      }
  }
  else if( 0 == str.compare("show_ccu")){
    return 5;
  }
  else if( 0 == str.compare("show_module")){
    wprintw(command_win," input: ");
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
        wprintw(command_win," input: ");
        wgetstr(command_win,ch);
        dirname=ch;

        return 9;
  }
  else if(0 == str.compare("curdir")){
      if(isPMT){
        return -1;
      }
      else{
        wprintw(command_win," input: ");
        wgetstr(command_win,ch);
        CurrentDir=ch;

        return 10;
      }
  }
  else if(0 == str.compare("setdir")){
    if(isPMT){
        wprintw(command_win," input: ");
        wgetstr(command_win,ch);
        PMTdir=ch;
        return 11;
    }
    else{
        return -1;
    }
  }
  else if(0 == str.compare("connect")){
    if(isPMT){
        return 12;
    }
    else{
        return -1;
    }
  }
  else if( 0 == str.compare("config_pmt")){
      if(isPMT){
          return 13;
      }
      else{
          return -1;
      }
  }
  else if( 0 == str.compare("show_pmt")){
      if(isPMT){
          return 14;
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

string CDisplay::getDirname()
{
    return dirname;
}

string CDisplay::getCurrentDir()
{
    return CurrentDir;
}

string CDisplay::getPMTdir()
{
    return PMTdir;
}

string CDisplay::getModulename()
{
    return module_name;
}
