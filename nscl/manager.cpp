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
  welcome();
  
  lock_isStarted = false;
  lock_isConfiged = true;
  lock_isDaqQuited = true;

  m_hits = -1;
  pDisplay = NULL;
  pCCU = NULL;

  if( true == FirstLoad() )
    {
      pDisplay = new CDisplay();
      Config();
      pthread_create( &mDisplayThread , NULL , displayThread , this );

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
	    pDisplay->output("Error Command used.");
	    break;
	  }
	case 0:  // quit -------------------------------
	  {
	    pDisplay->output("Program is going to quit.");

        Sleep(1000);
	    CLog("Quit. ");
	    return;
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

	case 3:  // start daq cycle ---------------------
	  {
	    pDisplay->output("DAQ cycle is going to start.");

        if ( true == lock_isStarted )
	      {
		pDisplay->output("DAQ cycle is running now.");
	      }
	    else
	      {
		lock_isStarted = true;

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate( &attr ,
					     PTHREAD_CREATE_DETACHED);
		pthread_create( &mDaqThread , NULL , 
				daqThread , this );

		m_Daq_start = clock();
	      }
	    
	    break;
	  }
	case 4: // stop --------------------------------
	  {
	    if ( lock_isStarted )
	      {
		lock_isStarted = false;
		pDisplay->output("Waitting for DAQ quiting... ");
		while( false == lock_isDaqQuited );

		m_Daq_stop = clock();
		pDisplay->output("DAQ Stoped. ");

		double time = (double)( m_Daq_stop - m_Daq_start )
		  /(double)CLOCKS_PER_SEC;

		char buf1[100];
		pDisplay->output( "Statistics during last DAQ:" );  
		sprintf( buf1 , "    time = %f s" , time);
		pDisplay->output( buf1 );

		char buf2[100];
		sprintf( buf2 , "    hits = %d " , m_hits);
		pDisplay->output( buf2 );

        //close(slisten);
	      }
	    else
	      {
		pDisplay->output("No DAQ running. ");
     
	      }

	    break;
	  }

	}

      flag = 100;
      
    }
}


bool CManager::daqCycle()
{
  m_hits =  pADC->DaqCycle( &lock_isStarted );

  //  cout<< n<<endl;
  return true;
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
  delModules();

  NSCLmodule* tempmodule;
  int size=config_module.size();
  for(int i=0;i<size;i++){
      tempmodule=new NSCLmodule(pCCU);
      tempmodule->config(*config_module[i]);
      modules.push_back(tempmodule);
  }

  flag = pADC->config( config_adc ) ;
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

  // Write the Configs of ADC to files ------------------------
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

  return true;

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

bool CManager::ConfigLoad()
{
  if(!CcuLoad())    return false;
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
	  pMan->pDisplay->form( pMan->pADC , pMan->lock_isConfiged);

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
  pMan->lock_isStarted = false;

  pMan->lock_isDaqQuited = true;

  return NULL;
}



