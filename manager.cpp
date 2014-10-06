/***************************************************
# File Name:	manager.cpp
# Abstract:	
# Author:	zhangzhelucky
# Update History:
Wed May  8 14:17:58 2013 Take it from main.cpp
****************************************************/


#include "manager.h"
#include "log.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <windows.h>

using namespace std;

CManager::CManager()
{
  system("cls");

  mVersion = "0.1.1";  // Initialize Software version number
  filename ="";
  CurDir="./";
  welcome();
  
  lock_isStarted = false;
  lock_isConfiged = true;
  lock_isDaqQuited = true;

  //PMT testing//////////
  PMTdir="";
  isPMT=false;
  isPMTConfiged=false;
  PulserStatus=UNCON;
  HVStatus=UNCON;
  packet_num=100;
  warming_seconds=60*10;
  stablizing_seconds=60*2;
  current_limit = 500.0;
  warming_voltage = 500.0;
  pHVController=NULL;
  pPulser=NULL;
  /////////////////////////

  m_hits = -1;
  pDisplay = NULL;
  pCCU = NULL;

  //if( true == FirstLoad() )
    {
      pDisplay = new CDisplay();
      //Config();
      CmdAnalyse();
    }
}

CManager::~CManager()
{
  if( pCCU )
    delete pCCU;
  if( pDisplay )
    delete pDisplay;
  delModules();
  delModuleConfig();
  //PMT
  delPMTConfig();
}

void
CManager::delModules()
{
    int size=modules.size();
    if(size>0){
        for(int i=0;i<size;i++){
            delete modules[i];
        }
    }
    modules.clear();
}

void
CManager::delModuleConfig()
{
    int size=config_module.size();
    if(size>0){
        for(int i=0;i<size;i++){
            delete config_module[i];
        }
    }
    config_module.clear();
}

void CManager::CmdAnalyse()
{
  while(1)
    {
      int flag = pDisplay->getCmd();
      switch( flag )
        {
	case -1: // error commands ---------------------
	  {
          if(isPMT)
                pDisplay->output("Error Command.You're in PMT mode.",CDisplay::WARNING_T);
          else
                pDisplay->output("Error Command.You're in Normal mode",CDisplay::WARNING_T);
	    break;
	  }
	case 0:  // quit -------------------------------
	  {
          if(lock_isStarted){
            pDisplay->output("DAQ cycle is running now.Stop it first",CDisplay::WARNING_T);
          }
          else{
            pDisplay->output("Program is going to quit.",CDisplay::NORMAL_T);

            Sleep(1000);
            CLog("Quit. ");
            return;
          }
	    break;
	  }
	case 1:  // config -----------------------------
	  {
	    if ( true == lock_isStarted )
	      {
            pDisplay->output("Can't config while running. ",CDisplay::WARNING_T);
	      }
	    else
	      {
            if ( true == Config() )
		  {
            pDisplay->output("Config Done. ",CDisplay::PMT_T);
		    lock_isConfiged = true;
		  }
            else
		  {
            pDisplay->output("Config Failed. ",CDisplay::ERROR_T);
            //return;
		  }
	      }
	    break;
	  }
    case 2:  // start daq cycle ---------------------
	  {
          if(isPMT){
              if(!isPMTConfiged){
                  pDisplay->output("Please config the testing procedure before going",CDisplay::WARNING_T);
                  pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"Please config the testing procedure before going");
              }
              else if((PulserStatus != SUCCESS) || (HVStatus != SUCCESS)){
                  pDisplay->output("Please check the connection and re-config",CDisplay::WARNING_T);
                  pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"Please check the connection and re-config");
              }
              else if(PMTdir.empty()){
                  pDisplay->output("Please set testing directory",CDisplay::WARNING_T);
                  pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"Please set testing directory");
              }
              else{
                  if(lock_isStarted){
                      pDisplay->output("PMT testing in progress.Wait for it completing",CDisplay::WARNING_T);
                  }
                  else{
                      lock_isStarted=true;
                      pDisplay->output("PMT testing is started.",CDisplay::PMT_T);
                      pDisplay->pmt_status(false,PulserStatus,HVStatus,PMTdir.c_str(),"PMT testing is started.");

                      pthread_attr_t attr;
                      pthread_attr_init(&attr);
                      pthread_attr_setdetachstate( &attr ,
                           PTHREAD_CREATE_DETACHED);
                      int err=pthread_create( &mPMTTestingThread , &attr,
                          pmtTestingThread , this );
                      if(err!=0){
                          string tempstr="can't create thread: ";
                          tempstr.append(strerror(err));
                          pDisplay->output(tempstr,CDisplay::ERROR_T);
                      }
                      pthread_attr_destroy(&attr);
                  }
              }
          }
          else{
            if(filename.empty()){
                pDisplay->output("Please set the output filename first",CDisplay::WARNING_T);
            }
            else{
                pDisplay->output("DAQ cycle is going to start.",CDisplay::PMT_T);

                if ( true == lock_isStarted )
                {
                    pDisplay->output("DAQ cycle is running now.Stop it first",CDisplay::WARNING_T);
                }
                else
                {
                    lock_isStarted = true;
                    pDisplay->output("DAQ cycle is started.",CDisplay::PMT_T);
                    pDisplay->normal_status(false,CurDir.c_str(),filename.c_str(),"DAQ cycle is started.");

                    pthread_attr_t attr;
                    pthread_attr_init(&attr);
                    pthread_attr_setdetachstate( &attr ,
					     PTHREAD_CREATE_DETACHED);
                    int err=pthread_create( &mDaqThread , &attr ,
                        daqThread , this );
                    if(err!=0){
                        string tempstr="can't create thread: ";
                        tempstr.append(strerror(err));
                        pDisplay->output(tempstr,CDisplay::ERROR_T);
                    }
                    pthread_attr_destroy(&attr);
                    m_Daq_start = clock();
                }
            }
          }
          break;
	  }
    case 3: // stop --------------------------------
	  {
          if(isPMT){
            if(lock_isStarted){
                lock_isStarted = false;
                pDisplay->output("Wainting for PMT testing teminating.Data may be invalid",CDisplay::WARNING_T);
                pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"Wainting for PMT testing teminating.Data may be invalid");
                while( false == lock_isDaqQuited );

                pDisplay->output("PMT testing Stopped forcfully. ",CDisplay::PMT_T);
                pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"PMT testing Stopped forcfully.");
            }
            else{
                pDisplay->output("No PMT testing in run",CDisplay::PMT_T);
            }
          }
          else{
            if ( lock_isStarted )
            {
                lock_isStarted = false;
                pDisplay->output("Waitting for DAQ quiting... ",CDisplay::PMT_T);
                pDisplay->normal_status(false,CurDir.c_str(),filename.c_str(),"Waitting for DAQ quiting... ");
                while( false == lock_isDaqQuited );

                string tempstr;
                m_Daq_stop = clock();
                pDisplay->output("DAQ Stoped. ",CDisplay::PMT_T);
                tempstr="DAQ stopped successfully\n ";
                pDisplay->normal_status(true,CurDir.c_str(),filename.c_str(),tempstr.c_str());

                double time = (double)( m_Daq_stop - m_Daq_start )
                                        /(double)CLOCKS_PER_SEC;

                char buf1[100];
                pDisplay->scroll_status("Statistics during last DAQ:");
                sprintf( buf1 , "    time = %f s" , time);
                pDisplay->scroll_status(buf1);

                char buf2[100];
                sprintf( buf2 , "    hits = %d " , m_hits);
                pDisplay->scroll_status(buf2);

            }
            else
            {
                pDisplay->output("No DAQ running. ",CDisplay::WARNING_T);
            }
        }
	    break;
	  }
        case 4:
      {
          if(lock_isStarted){
              pDisplay->output("DAQ cycle is running now.Stop it first",CDisplay::WARNING_T);
          }
          else{
              string tempstr;
              if(!filename.empty()){
                    stringstream ss;
                    ss<<"WARNING: previous filename\""<<filename<<"\""
                    <<" will be replaced";
                    pDisplay->output(ss.str().c_str(),CDisplay::WARNING_T);
                    tempstr=ss.str();
                    tempstr.append("\n ");
                }
                filename=pDisplay->getFilename();
                stringstream ss;
                ss<< "Data will be save to file: " << CurDir<<"/"<< filename;
                pDisplay->output(ss.str().c_str(),CDisplay::PMT_T);
                tempstr.append(ss.str());
                pDisplay->normal_status(true,CurDir.c_str(),filename.c_str(),tempstr.c_str());
          }
          break;
      }
      case 5:
      {
        if(lock_isStarted){
            pDisplay->output("DAQ cycle is running now.Stop it first",CDisplay::WARNING_T);
        }
        else{
            CC_Config cur_config_cc;
            cur_config_cc=pCCU->getConfig();
            pDisplay->formCCU(cur_config_cc,config_module);
        }
        break;
      }
      case 6:
      {
          if(lock_isStarted){
              pDisplay->output("DAQ cycle is running now.Stop it first",CDisplay::WARNING_T);
          }
          else{
              Module_Config cur_module_config;
              string tempstr;
              string module_name=pDisplay->getModulename();
              int size=modules.size();
              for(int i=0;i<size;i++){
                  tempstr=modules[i]->getName();
                  if(tempstr == module_name){
                      cur_module_config=modules[i]->getConfig();
                      pDisplay->formSingleModule(cur_module_config);
                      break;
                  }
              }
          }
          break;
      }
    case 7:
      {
          if(lock_isStarted){
              pDisplay->output("DAQ cycle is running now.Stop it first",CDisplay::WARNING_T);
          }
          else{
              isPMT=true;
              isPMTConfiged=false;
              PulserStatus=UNCON;
              HVStatus=UNCON;
              PMTdir="";
              pDisplay->output("you're in PMT testing mode now",CDisplay::PMT_T);
              pDisplay->output("Please config TESTING PROCEDUR and DIRECTORY first before proceeding",CDisplay::PMT_T);
              pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"Initial State in PMT testing");
          }
          break;
	}
      case 8:
      {
        if(lock_isStarted){
            pDisplay->output("PMT testing is running now.Stop it first",CDisplay::WARNING_T);
        }
        else{
            isPMT=false;
            delPMTConfig();
            filename="";
            pDisplay->output("you're in Normal testing mode now",CDisplay::PMT_T);
            pDisplay->normal_status(true,CurDir.c_str(),filename.c_str(),"Initial State in Normal testing");
        }
        break;
      }
      case 9:
      {
          if(lock_isStarted){
              pDisplay->output("DAQ cycle is running now.Stop it first",CDisplay::WARNING_T);
          }
          else{
            char msg[256];
            string dir=pDisplay->getDirname();
            if(!MkDir(dir.c_str(),msg)){
                string temp="Error! ";
                temp.append(msg);
                pDisplay->output(temp,CDisplay::ERROR_T);
                pDisplay->normal_status(true,CurDir.c_str(),filename.c_str(),temp.c_str());
            }
            else{
                string tempstr;
                tempstr="New Directory: "+ dir;
                pDisplay->output(tempstr,CDisplay::PMT_T);
                pDisplay->normal_status(true,CurDir.c_str(),filename.c_str(),tempstr.c_str());
            }
          }
          break;
      }
      case 10:
      {
          if(lock_isStarted){
              pDisplay->output("DAQ cycle is running now.Stop it first",CDisplay::WARNING_T);
          }
          else{
            CurDir=pDisplay->getCurrentDir();
            string tempstr="Files will be saved to directory: "+ CurDir;
            pDisplay->output(tempstr,CDisplay::PMT_T);
            pDisplay->normal_status(true,CurDir.c_str(),filename.c_str(),tempstr.c_str());
          }
          break;
      }
      case 11:
      {
          if(lock_isStarted){
              pDisplay->output("DAQ cycle is running now.Stop it first",CDisplay::WARNING_T);
          }
          else{
              string tempstr;
              if(!PMTdir.empty()){
                  stringstream ss;
                  ss<<"WARNING: previous Testing dir\""<<PMTdir<<"\""
                  <<" will be replaced";
                  pDisplay->output(ss.str().c_str(),CDisplay::WARNING_T);
                  tempstr=ss.str();
                  tempstr.append("\n ");
              }
              PMTdir=pDisplay->getPMTdir();
              stringstream ss;
              ss<< "New Testing Dir: " << PMTdir;
              pDisplay->output(ss.str().c_str(),CDisplay::PMT_T);
              tempstr.append(ss.str());
              pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),tempstr.c_str());
          }
          break;
      }
      case 12:
      {
          if(!isPMTConfiged){
              pDisplay->output("Config PMT testing procedure first",CDisplay::WARNING_T);
          }
          else{
              if(pPulser->Status()){
                  PulserStatus=SUCCESS;
              }
              else{
                  PulserStatus=FAILED;
              }
              if(pHVController->connect()){
                  Sleep(100);
                  if(pHVController->disConnect()){
                      HVStatus=SUCCESS;
                  }
                  else{
                      HVStatus=FAILED;
                  }
              }
              else{
                  HVStatus=FAILED;
              }
              if(HVStatus==SUCCESS && PulserStatus==SUCCESS)
                  pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"Connected");
              else if(HVStatus != SUCCESS && PulserStatus == SUCCESS)
                  pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"HV connection failed");
              else if(HVStatus == SUCCESS && PulserStatus != SUCCESS)
                  pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"Pulser connection failed");
              else
                  pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"Pulser and HV connection failed");
          }
          break;
      }
      case 13:
      {
          if(ConfigPMT()){
              isPMTConfiged=true;
              PulserStatus=UNCON;
              HVStatus=UNCON;
              CLog("Config PMT testing successfully");
              pDisplay->output("Config PMT-testing successfully",CDisplay::PMT_T);
              pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"Config PMT-testing successfully");
          }
          else{
              isPMTConfiged=false;
              PulserStatus=UNCON;
              HVStatus=UNCON;
              CLog("Config PMT testing failed");
              pDisplay->output("Config PMT-tesging failed",CDisplay::ERROR_T);
              pDisplay->pmt_status(true,PulserStatus,HVStatus,PMTdir.c_str(),"Config PMT-testing failed");
          }
          break;
      }
      case 14:
      {
          if(lock_isStarted){
            pDisplay->output("DAQ cycle is running now.Stop it first",CDisplay::WARNING_T);
          }
          else{
            if(!isPMTConfiged){
                pDisplay->output("config PMT testign first",CDisplay::WARNING_T);
            }
            else{
                pDisplay->formPMT(_formatPMTTesting());
            }
          }
          break;
      }
    }
      flag = 100;
      
    }
}

bool CManager::daqCycle()
{
  //init
    daqInit();
  //start stack execute
    stackStart();
  //open file
  char buffer[4096*2];
  size_t buffersize=4096*2;//ccusb need buffersize larger than the actual buffer length
  size_t transferCount=0;
  string tempname=CurDir+"/"+filename;
  FILE* fp=fopen(tempname.c_str(),"wb");
  if(fp == NULL){
      stringstream tempstr;
      tempstr<<"can't open file: "<< filename;
      pDisplay->output(tempstr.str().c_str(),CDisplay::ERROR_T);
      return false;
  }
  int status;
  size_t writeCount;
  size_t counter=0;
  while(lock_isStarted){
       status=pCCU->usbRead(buffer,buffersize,&transferCount);
       if(status<0){
           pDisplay->scroll_status("waiting data...");
       }
       if(transferCount>0){
           writeCount=fwrite(buffer,sizeof(char),transferCount,fp);
           if(writeCount != transferCount){
               pDisplay->output("data written error, DAQ cycle terminated!",CDisplay::ERROR_T);
               pDisplay->normal_status(false,CurDir.c_str(),filename.c_str(),"data written error, DAQ cycle terminated!");
               fclose(fp);
               stackStop();
               while(transferCount>0){
                   status=pCCU->usbRead(buffer,buffersize,&transferCount,3000);
               }
               daqClear();
               return false;
           }
           counter++;
           if(counter%100==0){
               char msg[100];
               sprintf(msg,"%d packets",counter);
               pDisplay->scroll_status(msg);
           }
       }
  }
  //stop stack execute
  stackStop();
  //read remaining packets
  transferCount=1;
  while(transferCount>0){
      status=pCCU->usbRead(buffer,buffersize,&transferCount,3000);
      if(transferCount>0){
          writeCount=fwrite(buffer,sizeof(char),transferCount,fp);
          if(writeCount != transferCount){
              pDisplay->output("CAUTION!: data written remaining error!",CDisplay::WARNING_T);
          }
      }
  }
  //clean procedure
  daqClear();

  //close file
  fclose(fp);
  return true;
}

void CManager::daqInit()
{
    //select event trigger as scalor_a trigger
    //pCCU->writeDeviceSourceSelectors(0x4);
    //enable scalor_a
    pCCU->writeDeviceSourceSelectors(0x10);
    //clear scalor_a
    pCCU->writeDeviceSourceSelectors(0x20);

    //eable LAM
    int size=modules.size();
    for(int i=0;i<size;i++){
        modules[i]->clrDataMem();
        modules[i]->enableLam();
    }

    return;
}

void CManager::daqClear()
{
    //disable scalor_a
    pCCU->writeDeviceSourceSelectors(0x40);
    //clear scalor_a
    pCCU->writeDeviceSourceSelectors(0x20);
    //disable LAM
    int size=modules.size();
    for(int i=0;i<size;i++){
        modules[i]->disableLam();
        modules[i]->clrDataMem();
    }

    return;
}

void CManager::stackStart()
{
    pCCU->writeActionRegister(0x1);
}

void CManager::stackStop()
{
    pCCU->writeActionRegister(0x0);
}

bool CManager::FirstLoad()
{
  string timestr = CManager::getTimeStr();
  //string tempstr = "log/log_" + timestr;
  string tempstr="log.txt";
  CLog::fileChar = tempstr;

  CLog( timestr.c_str() ,true);

  cout << "Time: " << timestr << endl;
  cout << "Your working will be logged to file " 
       << CLog::fileChar << endl << endl;

  if( false == CcusbDevFind() )
    {
      return false;
    }
  CcusbDevOpen();
  
  return true;
}

bool CManager::Config()
{
  // Load config files
  bool flag = ConfigLoad();
  if( true == flag )
    {
      CLog("Load Config files successfully. ");
      pDisplay->output("Load Config files successfully. ",CDisplay::PMT_T);
    }
  else
    {
      CLog("Failed in Loading Config files. ");
      pDisplay->output("Failed in Loading Config files. ",CDisplay::ERROR_T);
      return false;
    }

  // Config Devices =======================================
  // Config CCUsb --------------------------
  flag =  pCCU->config(config_cc);

  if( true == flag )
    {
      CLog("Set CCU successfully");
      pDisplay->output("Set CCU successfully. ",CDisplay::PMT_T);
    }
  else
    {
      CLog("Failed in setting CCU");
      pDisplay->output("Failed in setting CCU. ",CDisplay::ERROR_T);
      return false;
    }

  // Config ADC ----------------------------
  pCCU->z();
  delModules();

  flag=true;
  NSCLmodule* tempmodule;
  int size=config_module.size();
  for(int i=0;i<size;i++){
      tempmodule=new NSCLmodule(pCCU);
      if(!tempmodule->config(*config_module[i])) {
          flag=false;
          break;
      }
      modules.push_back(tempmodule);
  }

  if( true == flag )
    {
      CLog("Set ADC successfully");
      pDisplay->output("Set ADC successfully. ",CDisplay::PMT_T);
    }
  else
    {
      CLog("Failed in Loading ADC Configs");
      pDisplay->output("Failed in setting ADC. ",CDisplay::ERROR_T);
      return false;
    }

  // build stacklist and load
  buildStack();
  pCCU->loadList(0,stacklist);

  // Write the Configs of ADC to files ------------------------
  /*
  sParm para[16];
  pADC->getParaRegAll( para );
  string paraStr;

  paraStr = "Adc setting : \nChannel\tUT\tLT\tPED\n";
  CLog( paraStr.c_str() );

  for (int i = 0; i < 16; ++i)
    {
      stringstream stream;
      stream << i << "+"
	     << para[i].UT << "+"
	     << para[i].LT << "+"
	     << para[i].PED;
      stream >>paraStr;

      int pos = -1;
      while( 1 )
	{
	  pos = paraStr.find( '+', pos+1 );
	  if ( string::npos == pos)  break;
	  paraStr.replace( pos , 1 , 1 , '\t');
	}
      CLog( paraStr.c_str() );
    }
*/
  //display config
  CC_Config cur_config_cc=pCCU->getConfig();
  pDisplay->formCCU(cur_config_cc,config_module);

  return true;
}

void CManager::buildStack()
{
    stacklist.clear();
    //read Scalor_A
    stacklist.addRead24(25,11,0);
    //read DataMemory
    int size,n,a,f;
    uint16_t max=16;
    size=modules.size();
    for(int i=0;i<size;i++){
        //qscan
        n=modules[i]->getStation();a=0;f=0;
        stacklist.addQScan(n,a,f,max,true);
        //clear lam and data memory
        a=3;f=11;
        stacklist.addControl(n,a,f);
    }
    //add event terminator
    stacklist.addWrite16(0,0,16,0xEEEE);
}

void CManager::welcome()
{
  cout << "Welcome! "<<endl
       << "PMT Testing DAQ " << mVersion <<endl
       << "Copyright 2014. IMP. Zhou Yong. "<<endl
       << "This is free software with ABSOLUTELY NO WARRANTY. " 
       << endl << endl;
}

bool CManager::CcusbDevFind()
{
    int mDevNumber;
    vector<struct usb_device*> devices;
    devices=CCCUSB::enumerate();
    mDevNumber=devices.size();

    if ( 0==mDevNumber )
    {
      cout << "No CCUsb device found. " << endl
	   << "Please check whether your CCUsb devices is Powered. "
	   << endl << "Press [ Enter ] to quit. " << endl;
      getchar();
      return false;
    }
  else
    {
      cout << mDevNumber << " Device(s) Found. "
	   <<"Serial Number List:"<<endl;

      string tempstr;
      for (int i = 0; i < mDevNumber ; ++i)
	{
      tempstr=CCCUSB::serialNo(devices[i]);
      cout << "    Dev-"<< i <<"\t"<< tempstr << endl;
	}
    } 

  return true;
}

bool CManager::CcusbDevOpen()
{
  cout << "Press [Enter] to OPEN device listed... " << endl;
  getchar();
  pCCU = new CCCUSB();
  CLog("Device Open. ");

  return true;
}

bool CManager::CcuLoad()
{
    string tempstr;
    uint32_t tempint;
    // Load cc configs
    ifstream fp;
    fp.open( CC_ConfigPath );
    fp>>hex;
    if( !fp )
      {
        return false;
      }
    config_cc.clear();

    while( 1 )
      {
        fp >> tempstr;
        if( "CONFIG_BEGIN" == tempstr )
      {
        break;
      }
      }

    while( "CONFIG_END" != tempstr )
      {
        fp >> tempstr;

        if( tempstr == "GlobalMode" )
      {
        fp >> tempstr;
        fp >> tempint;
        config_cc.setGlobalMode(tempint);
      }
        else if( tempstr == "Delays" )
      {
        fp >> tempstr;
        fp >>tempint;
        config_cc.setDelays(tempint);
      }
        else if( tempstr == "ScalReadCtrl" )
      {
        fp >> tempstr;
        fp >>tempint;
        config_cc.setScalReadCtrl(tempint);
      }
        else if( tempstr == "SelectLED" )
      {
        fp >> tempstr;
        fp >> tempint;
        config_cc.setSelectLED(tempint);
      }
        else if( tempstr == "SelectNIMO" )
      {
        fp >> tempstr;
        fp >> tempint;
        config_cc.setSelectNIMO(tempint);
      }
        else if( tempstr == "SelectUserDevice" )
      {
        fp >> tempstr;
        fp >> tempint;
        config_cc.setSelectUserDevice(tempint);
      }
        else if( tempstr == "TimingDGGA" )
      {
        fp >> tempstr;
        fp >> tempint;
        config_cc.setTimingDGGA(tempint);
      }
        else if( tempstr == "TimingDGGB" )
      {
        fp >> tempstr;
        fp >> tempint;
        config_cc.setTimingDGGB(tempint);
      }
        else if( tempstr == "LAMMask" )
      {
        fp >> tempstr;
        fp >> tempint;
        config_cc.setLAMMask(tempint);
      }
        else if( tempstr == "ExtendedDelay" )
      {
        fp >> tempstr;
        fp >> tempint;
        config_cc.setExtendedDelay(tempint);
      }

        else if( tempstr == "UsbBufferSetup" )
      {
        fp >> tempstr;
        fp >> tempint;
        config_cc.setUsbBufferSetup(tempint);
      }
      }
    fp.close();

    return true;
}

bool CManager::ModuleLoad()
{
    // Load adc configs
    ifstream fp;
    string tempstr;
    int tempint_2;
    int counter=0;
    Module_Config* tempconfig;
    //
    fp.open( ADC_ConfigPath );
    if( !fp )
    {
        return false;
    }

    // Clear previous ModuleConfigFactory
    delModuleConfig();
    //
    while( 1 )
    {
        fp >> tempstr;
        if( "CONFIG_BEGIN" == tempstr )
        {
            break;
        }
    }
    while("CONFIG_END" != tempstr)
    {
        fp >> tempstr;
        if(tempstr == "STATION")
        {
            counter++;
            tempconfig=new Module_Config();
            //first is station number
            fp >> tempstr;
            fp >> tempint_2;
            tempconfig->setStation(tempint_2);

            while(tempstr != "STATION_END")
            {
                fp >> tempstr;
                if(tempstr == "NAME"){
                    fp >> tempstr;
                    fp >> tempstr;
                    tempconfig->setName(tempstr);
                }
                else if(tempstr == "Channel"){
                    //discard header
                    while( 1 )
                    {
                        fp >> tempstr;
                        if( "Index" == tempstr )
                        {
                            break;
                        }
                    }
                    //read config data
                    tempint_2=99;
                    while(tempint_2 >=0)
                    {
                        fp >> tempint_2;

                        if ( 0 == tempint_2 )
                        {
                            uint16_t ctrl[3];
                            for (int i = 0; i < 3; ++i)
                            {
                                fp >> ( ctrl[ i ] );
                            }
                            uint16_t control=(ctrl[0]<<2) + (ctrl[1]<<1) + ctrl[2];
                            tempconfig->setCtrl(control);
                        }
                        else if( tempint_2 > 0 && tempint_2 <= 16 )
                        {
                            int tmp[4];
                            for (int i = 0; i < 4 ; ++i)
                            {
                                fp >> tmp[i];
                            }
                            tempconfig->setUT(tempint_2,tmp[0]);
                            tempconfig->setLT(tempint_2,tmp[1]);
                            tempconfig->setPED(tempint_2,tmp[2]);
                        }
                    }
                }
            }
            config_module.push_back(tempconfig);
        }
    }

    if(config_module.size() != counter) return false;

    fp.close();

    return true;
}

bool CManager::CcudDefaultConfig()
{
    config_cc.clear();
    //Global: 4096 buffer length,extra buffer header
    config_cc.setGlobalMode(0x100);
    //Delay:  100us LAM timeout,8us trigger delay
    config_cc.setDelays(0x640a);
    //DeviceSource: event trigger as scalor_a source
    config_cc.setSelectUserDevice(0x4);

    return true;
}

bool CManager::ConfigLoad()
{
  //if(!CcuLoad())    return false;
  if(!CcudDefaultConfig())  return false;
  if(!ModuleLoad()) return false;

  return true;
}

string CManager::getTimeStr()
{
  time_t lt = time(NULL);
  tm* current = localtime( &lt );
  char str[100];
  strftime( str , 100 , "%y-%m-%d_%X", current);

  return std::string(str);

}

void* CManager::displayThread( void* args )
{
  CManager* pMan = ( CManager* )args;

  clock_t start, now;
  start = clock();


  while( 1 )
    {
      now = clock();
      //      if ( ( CLOCKS_PER_SEC ) < ( now - start ) )
      if ( 2000 < ( now - start ) )
	{
	  start = clock();
      //pMan->pDisplay->form( pMan->pADC , pMan->lock_isConfiged);

	  pMan->lock_isConfiged = false;
	}

    }

  return NULL;
}

void* CManager::daqThread( void* args )
{
  CManager* pMan = ( CManager* )args;

  pMan->lock_isDaqQuited = false;

  pMan->daqCycle();

  pMan->filename.clear();
  pMan->lock_isStarted = false;
  pMan->lock_isDaqQuited = true;

  return NULL;
}

//pmt testing
bool CManager::MkDir(const char* dir,char *msg)
{
    if (!CreateDirectory(dir, NULL))
    {
        if(ERROR_ALREADY_EXISTS == GetLastError()){
          sprintf(msg,"%s already exists",PMTdir.c_str());
          return false;
       }
       else if(ERROR_PATH_NOT_FOUND == GetLastError()){
          sprintf(msg,"%s path not found",PMTdir.c_str());
          return false;
       }
    }
    return true;
}

void CManager::delPMTConfig()
{
    if(isPMTConfiged){
        //
        size_t num=pHVGroup.size();
        for(size_t i=0;i<num;i++){
            delete pHVGroup[i];
        }
        pHVGroup.clear();
        //
        config_pmt.clear();
        config_hv.clear();
        //
        delete pHVController;
        delete pPulser;
        //
        isPMTConfiged=false;
    }
}

bool CManager::ConfigPMT()
{
    //clean
    delPMTConfig();
    //reinit
    if(!_configAFG3252()){
        CLog("Failed in config AFG3252");
        return false;
    }
    if(!_configSYX527()){
        CLog("Failed in config SYX527");
        return false;
    }
    if(!_configTesting()){
        CLog("Failed in config PMT Tesing procedure");
        return false;
    }

    return true;
}

bool CManager::_configAFG3252()
{
    string IPAddr,name;
    string tempstr;
    ifstream fp;
    fp.open(PMTConfig_PATH);
    if(!fp){
        return false;
    }
    //
    while(1){
        fp>> tempstr;
        if(tempstr == "<AFG3252_BEGIN>"){
            break;
        }
    }

    while(tempstr != "</AFG3252_END>"){
        fp >> tempstr;
        if(tempstr == "IP"){
            fp >> tempstr;
            fp >> tempstr;
            IPAddr=tempstr;
        }
        else if(tempstr == "NAME"){
            fp >> tempstr;
            fp >> tempstr;
            name=tempstr;
        }
    }
    //
    pPulser=new AFG3252(name.c_str(),IPAddr.c_str());

    fp.close();
    return true;
}

bool CManager::_configSYX527()
{
    string IPAddr,UserName,PassWord;
    ushort slot,ch_id;
    string chname;
    HVChannel tempchannel;
    string tempstr;
    ifstream fp;
    fp.open(PMTConfig_PATH);
    if(!fp){
        return false;
    }
    //
    while(1){
        fp>> tempstr;
        if(tempstr == "<SYX527_BEGIN>"){
            break;
        }
    }

    while(tempstr != "</SYX527_END>"){
        fp >> tempstr;
        if(tempstr == "IP"){
            fp >> tempstr;
            fp >> tempstr;
            IPAddr=tempstr;
        }
        else if(tempstr == "USERNAME"){
            fp >> tempstr;
            fp >> tempstr;
            UserName=tempstr;
        }
        else if(tempstr == "PASSWORD"){
            fp >> tempstr;
            fp >> tempstr;
            PassWord=tempstr;
        }
        else if(tempstr == "<TABLE_BEGIN>"){
            fp >> tempstr;
            fp >> tempstr;
            fp >> tempstr;

            fp >> tempstr;
            while(tempstr != "</TABLE_END>"){
                //
                chname=tempstr;
                fp >> slot;
                fp >> ch_id;
                tempchannel.slot=slot;
                tempchannel.ch_id=ch_id;
                sprintf(tempchannel.ch_name,"%s",chname.c_str());
                config_hv[slot].push_back(tempchannel);
                //
                fp >> tempstr;
            }
        }
    }
    //
    pHVController=new SYX527(IPAddr.c_str(),UserName.c_str(),PassWord.c_str());
    SYX527_Module* tempmodule;
    HVGroup::iterator it;
    for(it=config_hv.begin();it!=config_hv.end();it++){
        tempmodule=new SYX527_Module(pHVController,it->first,it->second);
        tempmodule->updateChName();
        tempmodule->setRampUp(50.0);tempmodule->updateRampUp();
        tempmodule->setRampDown(50.0);tempmodule->updateRampDown();
        pHVGroup.push_back(tempmodule);
    }
    //
    fp.close();
    return true;
}

bool CManager::_configTesting()
{
    float voltage;
    LEDAmp templed;
    string tempstr;
    ifstream fp;
    fp.open(PMTConfig_PATH);
    if(!fp){
        return false;
    }
    //
    while(1){
        fp>> tempstr;
        if(tempstr == "<PMTTesting_BEGIN>"){
            break;
        }
    }

    while(tempstr != "</PMTTesting_END>"){
        fp >> tempstr;
        if(tempstr == "PACKET_NUMBER"){
            fp >> tempstr;
            fp >> packet_num;
        }
        else if(tempstr == "WARMING_TIME"){
            fp >> tempstr;
            fp >> warming_seconds;
        }
        else if(tempstr == "CURRENT_LIMIT"){
            fp >> tempstr;
            fp >> current_limit;
        }
        else if(tempstr == "WARMING_VOLTAGE"){
            fp >> tempstr;
            fp >> warming_voltage;
        }
        else if(tempstr == "<STEP_BEGIN>"){
            while(tempstr != "</STEP_END>"){
                fp >> tempstr;
                if(tempstr == "V0"){
                    fp >> tempstr;
                    fp >> voltage;
                }
                else if(tempstr == "<TABLE_BEGIN>"){
                    fp >> tempstr;
                    fp >> tempstr;
                    fp >> tempstr;
                    fp >> tempstr;
                    fp >> tempstr;
                    fp >> tempstr;

                    fp >> tempstr;
                    while(tempstr != "</TABLE_END>"){
                        fp >> templed.highV;
                        fp >> templed.frq;
                        fp >> templed.width;
                        fp >> templed.trigDelay;
                        fp >> templed.trigWidth;
                        config_pmt[voltage].push_back(templed);
                        //
                        fp >> tempstr;
                    }
                }
            }
        }
    }

    fp.close();
    return true;
}

void CManager::daqCycle(FILE* fp,unsigned long num)
{
    //init
    daqInit();
    //start stack execute
    stackStart();
    //open file
    char buffer[4096*2];
    size_t buffersize=4096*2;//ccusb need buffersize larger than the actual buffer length
    size_t transferCount=0;

    int status;
    unsigned long counter=0;
    while(counter<num){
         status=pCCU->usbRead(buffer,buffersize,&transferCount);
         if(status<0){
             pDisplay->output("waiting data...",CDisplay::PMT_T);
         }
         if(transferCount>0){
             counter++;
             fwrite(buffer,sizeof(char),transferCount,fp);
             if((counter%100) == 0){
                 char msg[100];
                 sprintf(msg,"%d packets",counter);
                 pDisplay->output(msg,CDisplay::PMT_T);
             }
         }
    }
    //stop stack execute
    stackStop();
    //read remaining packets
    transferCount=1;
    while(transferCount>0){
        status=pCCU->usbRead(buffer,buffersize,&transferCount,3000);
    }
    //clean procedure
    daqClear();

    return;
}

void CManager::pmtTesting()
{
    double interval;
    time_t begin,end;
    ofstream fp_config;
    ofstream fp_log;
    ofstream fp_ledconfig;
    FILE *fp_raw;

    string tempstr;
    stringstream tempss;
    tempss<<setprecision(2);
    //mk raw dir
    char msg[256];
    string hv_dir,raw_filename;
    string ledconfig_filename;
    string raw_dir=PMTdir+"/raw_data";
    if(!MkDir(raw_dir.c_str(),msg)){
        pDisplay->output(msg,CDisplay::ERROR_T);
        return;
    }

    //open log file and pmt config file
    string log_filename=raw_dir+"/log.txt";
    fp_log.open(log_filename.c_str());
    if(!fp_log){
        pDisplay->output("can't open "+log_filename,CDisplay::ERROR_T);
        return;
    }
    fp_log << "Logging info of this PMT testing" <<endl;

    string config_filename=raw_dir+"/pmt.conf";
    fp_config.open(config_filename.c_str());
    if(!fp_config){
        pDisplay->output("can't open "+config_filename,CDisplay::ERROR_T);
        return;
    }

    //power on sy1527 and pmt warming
    _setV(warming_voltage);
    _setI(current_limit);
    _powerOn();

    tempstr=getTimeStr() + ": Power On SY1527";
    pDisplay->output(tempstr,CDisplay::PMT_T);
    fp_log << tempstr <<endl;
    tempstr=getTimeStr() + ": PMT warming started.";
    pDisplay->output(tempstr,CDisplay::PMT_T);
    fp_log << getTimeStr() <<": PMT warming started.Warming Voltage is "<<warming_voltage<<"V"<<endl;

    begin=time(NULL);
    end=time(NULL);
    interval=difftime(end,begin);
    while(interval < warming_seconds){
        if(!lock_isStarted){
            _cleanUp();
            return;
        }
        Sleep(60000);
        end=time(NULL);
        interval=difftime(end,begin);
    }

    tempstr=getTimeStr() + ": PMT warming stopped.";
    pDisplay->output(tempstr,CDisplay::PMT_T);
    fp_log << tempstr <<endl;

    _HVfeedback();
    fp_log<< _formatHVGroup();

    //LED pulser init
    _PulserInit();

    //pedestal testing before LED sweep
    tempstr=raw_dir+"/pedestal";
    if(!MkDir(tempstr.c_str(),msg)){
        pDisplay->output(msg,CDisplay::ERROR_T);
        return;
    }

    raw_filename=raw_dir+"/pedestal/begin.dat";
    fp_raw=fopen(raw_filename.c_str(),"wb");
    if(!fp_raw){
        pDisplay->output("can't open "+raw_filename,CDisplay::ERROR_T);
        return;
    }
    tempstr=getTimeStr()+": Pedestal Testing before formal testing";
    fp_log<<tempstr<<endl;
    pDisplay->output(tempstr,CDisplay::PMT_T);

    pPulser->PowerOn(2);
    daqCycle(fp_raw,packet_num);
    fclose(fp_raw);
    pPulser->PowerOff(2);

    tempstr=getTimeStr()+": Pedestal Testing completed";
    fp_log<<tempstr<<endl;
    pDisplay->output(tempstr,CDisplay::PMT_T);

    //formal testing
    fp_log<<"\nFormal Testing"<<endl;
    size_t voltage_step=config_pmt.size();
    fp_config << "Voltage Step: "<<voltage_step<<endl;
    size_t led_step;
    int tempvolt;
    PMTTestingConfig::iterator it;
    LEDPulserConfig tempLEDConfig;

    for(it=config_pmt.begin();it!=config_pmt.end();it++){
        //set voltage
        _setV(it->first);
        tempvolt=static_cast<int>(it->first);
        fp_config<< tempvolt <<"V"<<endl;
        //mkdir
        tempss.clear();tempss.str("");
        tempss<<raw_dir<<"/"<<tempvolt<<"V";
        hv_dir=tempss.str();
        if(!MkDir(hv_dir.c_str(),msg)){
            pDisplay->output(msg,CDisplay::ERROR_T);
            return;
        }

        //new LED.config file
        ledconfig_filename=hv_dir+"/LED.config";
        fp_ledconfig.open(ledconfig_filename.c_str());
        if(!fp_ledconfig){
            pDisplay->output("can't open "+ledconfig_filename,CDisplay::ERROR_T);
            return;
        }

        //stablizing
        begin=time(NULL);
        end=time(NULL);
        interval=difftime(end,begin);
        while(interval < stablizing_seconds){
            if(!lock_isStarted){
                _cleanUp();
                return;
            }
            Sleep(60000);
            end=time(NULL);
            interval=difftime(end,begin);
        }

        //logging
        tempss.clear();tempss.str("");
        tempss<<getTimeStr()<<": HV is "<<tempvolt<<"V.Testing begin..."<<endl;
        tempstr=tempss.str();
        fp_log<<tempstr;
        pDisplay->output(tempstr,CDisplay::PMT_T);

        _HVfeedback();
        fp_log<< _formatHVGroup();

        //testing begin
        tempLEDConfig=it->second;
        led_step=tempLEDConfig.size();

        fp_ledconfig<<"Measurement was started at "<<getTimeStr()<<endl;
        fp_ledconfig<<"Total datapoints: "<<led_step<<endl;
        fp_ledconfig<<"Index\tFrq\tWidth\tHigh_Level\tLow_Level\tLeading\tTrailing"<<endl;

        for(size_t i=0;i<led_step;i++){
            //logging
            tempss.clear();tempss.str("");
            tempss<<tempvolt<<"V. LED Amp_"<<i+1;
            pDisplay->output(tempss.str(),CDisplay::PMT_T);
            //light source
            pPulser->SetFrequency(1,tempLEDConfig[i].frq);
            pPulser->SetVoltageHigh(1,tempLEDConfig[i].highV);
            pPulser->SetPulseWidth(1,tempLEDConfig[i].width);
            //gate(trigger)
            pPulser->SetPulseDelay(2,tempLEDConfig[i].trigDelay);
            pPulser->SetPulseWidth(2,tempLEDConfig[i].trigWidth);
            //poweron
            pPulser->PowerOn(1);
            pPulser->PowerOn(2);

            //raw file
            tempss.clear();tempss.str("");
            tempss<<hv_dir<<"/"<<i+1<<".dat";
            raw_filename=tempss.str();
            fp_raw=fopen(raw_filename.c_str(),"wb");
            if(!fp_raw){
                pDisplay->output("can't open "+raw_filename,CDisplay::ERROR_T);
                return;
            }

            //daq
            daqCycle(fp_raw,packet_num);
            fclose(fp_raw);
            if(!lock_isStarted){
                _cleanUp();
                return;
            }

            //poweroff LED source
            pPulser->PowerOff(1);
            pPulser->PowerOff(2);
            //logging
            fp_ledconfig<<i+1<<"\t";
            pPulser->GetFrequency(1,msg);fp_ledconfig<<msg<<"\t";
            pPulser->GetPulseWidth(1,msg);fp_ledconfig<<msg<<"\t";
            pPulser->GetVoltageHigh(1,msg);fp_ledconfig<<msg<<"\t";
            pPulser->GetVoltageLow(1,msg);fp_ledconfig<<msg<<"\t";
            pPulser->GetPulseLeadingEdge(1,msg);fp_ledconfig<<msg<<"\t";
            pPulser->GetPulseTrailingEdge(1,msg);fp_ledconfig<<msg<<endl;
        }

        //testing end
        fp_ledconfig<<endl;
        fp_ledconfig<<"Measurement was finished at "<<getTimeStr()<<endl;
        fp_ledconfig.close();

        //logging
        tempss.clear();tempss.str("");
        tempss<<getTimeStr()<<": "<<tempvolt<<"V testing complete."<<endl;
        tempstr=tempss.str();
        fp_log<<tempstr;
        pDisplay->output(tempstr,CDisplay::PMT_T);

        _HVfeedback();
        fp_log<< _formatHVGroup();
        //

    }

    fp_log<<"\nFormal testing completed.\n"<<endl;
    fp_config.close();

    //pedestal testing after LED sweep
    raw_filename=raw_dir+"/pedestal/end.dat";
    fp_raw=fopen(raw_filename.c_str(),"wb");
    if(!fp_raw){
        pDisplay->output("can't open "+raw_filename,CDisplay::ERROR_T);
        return;
    }
    tempstr=getTimeStr()+": Pedestal Testing after formal testing";
    fp_log<<tempstr<<endl;
    pDisplay->output(tempstr,CDisplay::PMT_T);

    pPulser->PowerOn(2);
    daqCycle(fp_raw,packet_num);
    fclose(fp_raw);
    pPulser->PowerOff(2);

    tempstr=getTimeStr()+": Pedestal Testing completed";
    fp_log<<tempstr<<endl;
    pDisplay->output(tempstr,CDisplay::PMT_T);

    //Power off SY1527
    _powerOff();
    tempstr=getTimeStr() + ": Power Off SY1527";
    pDisplay->output(tempstr,CDisplay::PMT_T);
    fp_log << tempstr <<endl;

    fp_log.close();

}

void* CManager::pmtTestingThread(void *args)
{
    CManager* pMan = ( CManager* )args;
    //
    int err=pthread_create( &pMan->mHVmonitorThread , NULL,
        HVMonitorThread , args );
    if(err!=0){
        string tempstr="can't create HV thread: ";
        tempstr.append(strerror(err));
        pMan->pDisplay->output(tempstr,CDisplay::ERROR_T);
    }

    pMan->lock_isDaqQuited = false;

    pMan->pmtTesting();

    pthread_cancel(pMan->mHVmonitorThread);
    pthread_join(pMan->mHVmonitorThread,NULL);

    pMan->PMTdir.clear();
    pMan->lock_isStarted = false;
    pMan->lock_isDaqQuited = true;

    pMan->pDisplay->pmt_status(true,pMan->PulserStatus,pMan->HVStatus,pMan->PMTdir.c_str(),"PMT testing end");

    return NULL;
}

void* CManager::HVMonitorThread(void *args)
{
    CManager* pMan=(CManager*)args;

    while(1){
        pMan->_HVfeedback();
        pMan->pDisplay->formHV(pMan->config_hv);
        //
        pthread_testcancel();
        //
        Sleep(1500);
    }
}


void CManager::_setV(float voltage)
{
    size_t size=pHVGroup.size();
    for(size_t i=0;i<size;i++){
        pHVGroup[i]->setVSet(voltage);
        while(!pHVGroup[i]->updateVSet())
        {
            Sleep(100);
        }
    }
}

void CManager::_setI(float current)
{
    size_t size=pHVGroup.size();
    for(size_t i=0;i<size;i++){
        pHVGroup[i]->setISet(current);
        while(!pHVGroup[i]->updateISet())
        {
            Sleep(100);
        }
    }
}

void CManager::_setRup(float rup)
{
    size_t size=pHVGroup.size();
    for(size_t i=0;i<size;i++){
        pHVGroup[i]->setRampUp(rup);
        while(!pHVGroup[i]->updateRampUp()){
            Sleep(100);
        }
    }
}

void CManager::_setRDwn(float rdwn)
{
    size_t size=pHVGroup.size();
    for(size_t i=0;i<size;i++){
        pHVGroup[i]->setRampDown(rdwn);
        while(!pHVGroup[i]->updateRampDown())
        {
            Sleep(100);
        }
    }
}

void CManager::_powerOn()
{
    size_t size=pHVGroup.size();
    for(size_t i=0;i<size;i++){
        while(!pHVGroup[i]->PowerOn())
        {
            Sleep(100);
        }
    }
}

void CManager::_powerOff()
{
    size_t size=pHVGroup.size();
    for(size_t i=0;i<size;i++){
        while(!pHVGroup[i]->PowerOff())
        {
            Sleep(100);
        }
    }
}

void CManager::_HVfeedback()
{
    size_t size=pHVGroup.size();
    for(size_t i=0;i<size;i++){
        pHVGroup[i]->update(config_hv[pHVGroup[i]->getSlot()]);
    }
}

string CManager::_formatHVGroup()
{
    HVGroup::iterator it;
    HVChannels tempChs;
    size_t channels_size;
    stringstream ss;
    ss<<"\t\tCurrent Monitoring Value:\n";

    ss<<"\t\tCH:";
    for(it=config_hv.begin();it!=config_hv.end();it++){
        tempChs=it->second;
        channels_size=tempChs.size();
        for(size_t j=0;j<channels_size;j++){
            ss<<"\t"<<tempChs[j].slot<<"_"<<tempChs[j].ch_id;
        }
    }
    ss<<"\n";

    //ss<<setprecision(2);
    ss<<"\t\tVMon:";
    for(it=config_hv.begin();it!=config_hv.end();it++){
        tempChs=it->second;
        channels_size=tempChs.size();
        for(size_t j=0;j<channels_size;j++){
            ss<<"\t"<<tempChs[j].VMon;
        }
    }
    ss<<"\n";

    ss<<"\t\tIMon:";
    for(it=config_hv.begin();it!=config_hv.end();it++){
        tempChs=it->second;
        channels_size=tempChs.size();
        for(size_t j=0;j<channels_size;j++){
            ss<<"\t"<<tempChs[j].IMon;
        }
    }
    ss<<"\n";

    return ss.str();
}

string CManager::_formatPMTTesting()
{
    stringstream tempss;
    //
    tempss<<" SY1527 IP: "<<pHVController->getIP()<<endl;
    //
    tempss<<endl;
    tempss<<" "<<pPulser->GetName()<<" IP: "<<pPulser->GetIP()<<endl;
    //
    tempss<<endl;
    tempss<<" PMT-testing Info:"<<endl;
    tempss<<" Warming time: "<< warming_seconds/60<<"min"<<endl;
    tempss<<" Current limit: "<< current_limit <<"uA"<<endl;
    tempss<<" Packet_num/transfer: "<< packet_num <<endl;
    tempss<<" Voltage Step: "<<config_pmt.size()<<endl;
    PMTTestingConfig::iterator it;
    for(it=config_pmt.begin();it!=config_pmt.end();it++){
        tempss<<" "<<it->first<<"V"<<endl;
    }

    return tempss.str();

}

void CManager::_PulserInit()
{
    //Reset
    pPulser->Reset();
    pPulser->PowerOff(1);pPulser->PowerOff(2);
    //set shape
    pPulser->SetShape(1);pPulser->SetShape(2);
    //sychronize
    pPulser->TurnOnFrqConcurrent();
    pPulser->SetFrequency(1,1000);//IMPORTANT:set frequecy must befor setpolarity
    //pPulser->PhaseInitiate();

    //output2 is gate NIM
    pPulser->SetPolarity(2,false);
    pPulser->SetVoltageHigh(2,0);
    pPulser->SetVoltageLow(2,-0.8);
    pPulser->SetPulseWidth(2,200);
    //output1 is positive
    pPulser->SetVoltageLow(1,0);
}

void CManager::_cleanUp()
{
    _powerOff();
    pPulser->PowerOff(1);
    pPulser->PowerOff(2);
}
