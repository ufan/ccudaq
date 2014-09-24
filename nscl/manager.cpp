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
#include "stdio.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <windows.h>

using namespace std;

CManager::CManager()
{
  system("cls");

  mVersion = "0.1.1";  // Initialize Software version number
  filename ="";
  PMTdir="";
  welcome();
  
  lock_isStarted = false;
  lock_isConfiged = true;
  lock_isDaqQuited = true;

  isPMT=false;
  m_hits = -1;
  pDisplay = NULL;
  pCCU = NULL;

  if( true == FirstLoad() )
    {
      pDisplay = new CDisplay();
      Config();
      //pthread_create( &mDisplayThread , NULL , displayThread , this );

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
                pDisplay->output("Error Command.You're in PMT mode.");
          else
                pDisplay->output("Error Command.You're in Normal mode");
	    break;
	  }
	case 0:  // quit -------------------------------
	  {
	    pDisplay->output("Program is going to quit.");

        Sleep(1000);
	    CLog("Quit. ");
	    break;
	  }
	case 1:  // config -----------------------------
	  {
	    if ( true == lock_isStarted )
	      {
            pDisplay->output("Can't config while running. ");
	      }
	    else
	      {
            if ( true == Config() )
		  {
            pDisplay->output("Config Done. ");
		    lock_isConfiged = true;
		  }
            else
		  {
		    pDisplay->output("Config Failed. ");
		    return;
		  }
	      }
	    break;
	  }
    case 2:  // start daq cycle ---------------------
	  {
          if(isPMT){

          }
          else{
            if(filename.empty()){
                pDisplay->output("Please set the output filename first");
            }
            else{
                pDisplay->output("DAQ cycle is going to start.");

                if ( true == lock_isStarted )
                {
                    pDisplay->output("DAQ cycle is running now.Stop it first");
                }
                else
                {
                    lock_isStarted = true;
                    pDisplay->output("DAQ cycle is started.");
                    pDisplay->normal_status(false,filename.c_str(),"DAQ cycle is started.");

                    pthread_attr_t attr;
                    pthread_attr_init(&attr);
                    pthread_attr_setdetachstate( &attr ,
					     PTHREAD_CREATE_DETACHED);
                    pthread_create( &mDaqThread , NULL ,
                        daqThread , this );

                    m_Daq_start = clock();
                }
            }
          }
          break;
	  }
    case 3: // stop --------------------------------
	  {
          if(isPMT){

          }
          else{
            if ( lock_isStarted )
            {
                lock_isStarted = false;
                pDisplay->output("Waitting for DAQ quiting... ");
                //pDisplay->normal_status(false,filename.c_str(),"Waitting for DAQ quiting... ");
                while( false == lock_isDaqQuited );

                string tempstr;
                m_Daq_stop = clock();
                pDisplay->output("DAQ Stoped. ");
                tempstr="DAQ stopped successfully\n ";

                double time = (double)( m_Daq_stop - m_Daq_start )
                                        /(double)CLOCKS_PER_SEC;

                char buf1[100];
                pDisplay->output( "Statistics during last DAQ:" );
                sprintf( buf1 , "    time = %f s" , time);
                pDisplay->output( buf1 );
                tempstr.append(buf1);
                tempstr+="\n ";

                char buf2[100];
                sprintf( buf2 , "    hits = %d " , m_hits);
                pDisplay->output( buf2 );
                tempstr+=buf2;
                pDisplay->normal_status(true,NULL,tempstr.c_str());
	      }
          else
	      {
                pDisplay->output("No DAQ running. ");
	      }
        }
	    break;
	  }
        case 4:
      {
          if(lock_isStarted){
              pDisplay->output("DAQ cycle is running now.Stop it first");
          }
          else{
              string tempstr;
              if(!filename.empty()){
                    stringstream ss;
                    ss<<"WARNING: previous filename\""<<filename<<"\""
                    <<" will be replaced";
                    pDisplay->output(ss.str().c_str());
                    tempstr=ss.str();
                    tempstr.append("\n ");
                }
                filename=pDisplay->getFilename();
                stringstream ss;
                ss<< "Data will be save to file: " << filename;
                pDisplay->output(ss.str().c_str());
                tempstr.append(ss.str());
                pDisplay->normal_status(true,filename.c_str(),tempstr.c_str());
          }
          break;
      }
      case 5:
      {
        if(lock_isStarted){
            pDisplay->output("DAQ cycle is running now.Stop it first");
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
              pDisplay->output("DAQ cycle is running now.Stop it first");
          }
          else{
              Module_Config cur_module_config;
              string tempstr;
              string module_name=pDisplay->getModulename();
              int size=modules.size();
              for(int i=0;i<size;i++){
                  tempstr=modules[i].getName();
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
              pDisplay->output("DAQ cycle is running now.Stop it first");
          }
          else{
              isPMT=true;
              pDisplay->output("you're in PMT testing mode now");
              pDisplay->pmt_status(true,0,0,NULL,"Initial State in PMT testing");
          }
          break;
	}
      case 8:
      {
        if(lock_isStarted){
            pDisplay->output("DAQ cycle is running now.Stop it first");
        }
        else{
            isPMT=false;
            pDisplay->output("you're in Normal testing mode now");
            pDisplay->normal_status(true,NULL,"Initial State in Normal testing");
        }
        break;
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
  FILE* fp=fopen(filename.c_str(),"wb");
  if(fp == NULL){
      stringstream tempstr;
      tempstr<<"can't open file: "<< filename;
      pDisplay->output(tempstr.str().c_str());
      return false;
  }
  int status;
  size_t writeCount;
  while(lock_isStarted){
       status=pCCU->usbRead(buffer,buffersize,&transferCount);
       if(status<0){
           pDisplay->output("waiting data...");
       }
       if(transferCount>0){
           writeCount=fwrite(buffer,sizeof(char),transferCount,fp);
           if(writeCount != transferCount){
               pDisplay->output("data written error, DAQ cycle terminated!");
               fclose(fp);
               stackStop();
               while(transferCount>0){
                   status=pCCU->usbRead(buffer,buffersize,&transferCount,3000);
               }
               daqClear();
               return false;
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
              pDisplay->output("CAUTION!: data written remaining error!");
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
      pDisplay->output("Load Config files successfully. ");
    }
  else
    {
      CLog("Failed in Loading Config files. ");
      pDisplay->output("Failed in Loading Config files. ");
      return false;
    }

  // Config Devices =======================================
  // Config CCUsb --------------------------
  flag =  pCCU->config(config_cc);

  if( true == flag )
    {
      CLog("Set CCU successfully");
      pDisplay->output("Set CCU successfully. ");
    }
  else
    {
      CLog("Failed in setting CCU");
      pDisplay->output("Failed in setting CCU. ");
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
      pDisplay->output("Set ADC successfully. ");
    }
  else
    {
      CLog("Failed in Loading ADC Configs");
      pDisplay->output("Failed in setting ADC. ");
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
    config_cc.setDelays(0x6408);
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



