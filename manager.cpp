/***************************************************
# File Name:	manager.cpp
# Abstract:	
# Author:	zhangzhelucky
# Update History:
Wed May  8 14:17:58 2013 Take it from main.cpp
****************************************************/

// from YongSheng--------------------
#include <stdio.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <netinet/in.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/wait.h>  
#include <sys/time.h>  
#include <sys/types.h>  
#include <string.h>

struct CLIENT {
  int fd;
  struct sockaddr_in addr;
} ;

CLIENT client[FD_SETSIZE];
int nready;
int slisten, sockfd, maxfd = -1, connectfd;
unsigned int myport, lisnum;
struct sockaddr_in my_addr, addr;
struct timeval tv;

socklen_t len;
fd_set rset, allset;
int n, maxi = -1;
void SendData(int data)
{
int temp=data;
  
  for (int i = 0; i < FD_SETSIZE; i++) 
    {
      if (client[i].fd >-1) 
	{
	  if (send(client[i].fd,&temp,2, 0)<0)
	    {
	      close(client[i].fd);
	      FD_CLR(client[i].fd, &allset);
	      client[i].fd = -1;
	     
	    }
	}
    }
}
char buf[100 + 1];

// ----------------------------------


#include "manager.h"
#include "log.h"

#include <iostream>
#include "stdio.h"
#include <fstream>
#include <sstream>

using namespace std;

CManager::CManager()
{
  system("clear");

  mVersion = "0.0.1";  // Initialize Software version number
  welcome();
  
  lock_isChecked = false;
  lock_isStarted = false;
  lock_isConfiged = true;
  lock_isDaqQuited = true;

  isBind = false;
  m_hits = -1;
  pDisplay = NULL;
  pADC = NULL;
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
  if( pADC )
    delete pADC;
  if( pCCU )
    delete pCCU;
  if( pDisplay )
    delete pDisplay;
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

	    sleep(1);
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
		    pDisplay->output("Config Down. ");
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

	case 2:  // Check guests ------------------------
	  {

	    if ( lock_isStarted )
	      {
		pDisplay->output("Can't check while running. ");
	      }
	    else
	      {
		pDisplay->output("Checking ... ");	    
		// Add codes to check guests here =====================
	    
		if ( ! isBind )
		  {
		    isBind = true;
		    myport = 1234;
		    lisnum = FD_SETSIZE;
		    if ((slisten = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		      {
			perror("socket");
			exit(1);
		      }
		    bzero(&my_addr, sizeof (my_addr));
		    my_addr.sin_family = AF_INET;
		    my_addr.sin_port = htons(myport);
		    my_addr.sin_addr.s_addr =INADDR_ANY;//inet_addr("192.168.1.101");
		    if (bind(slisten, (struct sockaddr *) &my_addr, sizeof (my_addr)) == -1)
		      {
			perror("bind");
			exit(1);
		      }
		    if (listen(slisten, lisnum) == -1)
		      {
			perror("listen");
			exit(1);
		      }
		    for (int i = 0; i < FD_SETSIZE; i++)
		      {
			client[i].fd = -1;
		      }
		    FD_ZERO(&allset);
		    FD_SET(slisten, &allset);
		    maxfd = slisten;
		    pDisplay->output("Waiting for connections ...");
		  }

		for(int i=0;i<3000;i++)
		  {
		    rset = allset;
		    tv.tv_sec = 0;
		    tv.tv_usec = 10;
		    nready = select(maxfd + 1, &rset, NULL, NULL, &tv);
		    if (nready == 0)
		      continue;
		    else if (nready < 0) 
		      {
			pDisplay->output("select failed!");
			break;
		      }
		    else
		      {
			if (FD_ISSET(slisten, &rset)) // new connection  
			  {
			    len = sizeof (struct sockaddr);
			    if ((connectfd = accept(slisten,
						    (struct sockaddr*) &addr, &len)) == -1) 
			      {
				perror("accept() error\n");
				continue;
			      }
			    for (i = 0; i < FD_SETSIZE; i++) 
			      {
				if (client[i].fd < 0) 
				  {
				    client[i].fd = connectfd;
				    client[i].addr = addr;

				    string tempstr = inet_ntoa(client[i].addr.sin_addr);
				    tempstr = string("Connection from ") + tempstr;
				    short TestData=4096;
				    send(client[i].fd, &TestData,2, 0);
				    break;
				  }
			      }
			    if (i == FD_SETSIZE)
			      pDisplay->output( "over connections" );

			    FD_SET(connectfd, &allset);
			    if (connectfd > maxfd)
			      maxfd = connectfd;
			    if (i > maxi)
			      maxi = i;
			  }
			else 
			  {
			    for (i = 0; i <= maxi; i++)
			      {
				if ((sockfd = client[i].fd) < 0)
				  continue;
				if (FD_ISSET(sockfd, &rset)) 
				  {
				    bzero(buf, 100 + 1);
				    if ((n = recv(sockfd, buf, 100, 0)) > 0)
				      {

				      }
				    else 
				      {

					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i].fd = -1;
				      }
				  }
			      }
			  }
		      }
		  }
		for (int i = 0; i < FD_SETSIZE; i++) 
		  {
		    if (client[i].fd > -1) 
		      {
			string tempstr = inet_ntoa(client[i].addr.sin_addr);
			tempstr = string("Connection from ") + tempstr;
			pDisplay->output( tempstr );

		      }
		  }


		pDisplay->output( "listen stop" );

		lock_isChecked = true;
	      }
	    break;

	  }

	case 3:  // start daq cycle ---------------------
	  {
	    pDisplay->output("DAQ cycle is going to start.");

	    if ( false == lock_isChecked )
	      {
		pDisplay->output("Please check guests before start daq.");
	      }
	    else if ( true == lock_isStarted )
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

		close(slisten);    
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
  string tempstr = "log/log_" + timestr;
  CLog::fileChar = tempstr.c_str();

  CLog( timestr.c_str() );

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
  pCCU->CamacZ();

  flag =  pCCU->Config( config_cc );
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
  if ( pADC )
    {
      delete pADC;
    }

  pADC = new CAdc( pCCU , config_adc.station );

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
       << "DAQ " << mVersion <<endl
       << "Copyright 2013. PG. ZhangZhelucky, Hrb. "<<endl
       << "This is free software with ABSOLUTELY NO WARRANTY. " 
       << endl << endl;
}


bool CManager::CcusbDevFind()
{
  mDevNumber = xxusb_devices_find( pDeviceList );

  if ( mDevNumber<0 )
    {
      cout << "Failed in finding devices. " << endl
	   << "Please check your USB connect with CCUsb devices. " << endl
	   << "Press [ Enter ] to quit. " << endl;
      getchar();
      return false;
    }
  else if ( 0==mDevNumber )
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

      for (int i = 0; i < mDevNumber ; ++i)
	{
	  cout << "    Dev-"<< i <<"\t"<< (pDeviceList+i)->SerialString << endl;
	}
    } 

  return true;
}


bool CManager::CcusbDevOpen()
{
  cout << "Press [Enter] to OPEN device listed... " << endl;
  getchar();
  pCCU = new CCcusb( pDeviceList[0] );
  CLog("Device Open. ");

  return true;
}


bool CManager::ConfigLoad()
{
  string tempstr;
  int tempint;
  // Load cc configs
  ifstream fp;
  fp.open( CC_ConfigPath );
  if( !fp )
    {
      return false;
    }

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
	  config_cc.GlobalMode = tempint;
	}
      else if( tempstr == "Delays" )
	{
	  fp >> tempstr;
	  fp >> tempint;
	  config_cc.Delays = tempint;
	}
      else if( tempstr == "ScalReadCtrl" )
	{
	  fp >> tempstr;
	  fp >> tempint;
	  config_cc.ScalReadCtrl = tempint;
	}
      else if( tempstr == "SelectLED" )
	{
	  fp >> tempstr;
	  fp >> tempint;
	  config_cc.SelectLED = tempint;
	}
      else if( tempstr == "SelectNIMO" )
	{
	  fp >> tempstr;
	  fp >> tempint;
	  config_cc.SelectNIMO = tempint;
	}
      else if( tempstr == "SelectUserDevice" )
	{
	  fp >> tempstr;
	  fp >> tempint;
	  config_cc.SelectUserDevice = tempint;
	}
      else if( tempstr == "TimingDGGA" )
	{
	  fp >> tempstr;
	  fp >> tempint;
	  config_cc.TimingDGGA = tempint;
	}
      else if( tempstr == "TimingDGGB" )
	{
	  fp >> tempstr;
	  fp >> tempint;
	  config_cc.TimingDGGB = tempint;
	}
      else if( tempstr == "LAMMask" )
	{
	  fp >> tempstr;
	  fp >> tempint;
	  config_cc.LAMMask = tempint;
	}
      else if( tempstr == "ExtendedDelay" )
	{
	  fp >> tempstr;
	  fp >> tempint;
	  config_cc.ExtendedDelay = tempint;
	}

      else if( tempstr == "UsbBufferSetup" )
	{
	  fp >> tempstr;
	  fp >> tempint;
	  config_cc.UsbBufferSetup = tempint;
	}
    }
  fp.close();

  // Load adc configs
  fp.open( ADC_ConfigPath );
  if( !fp )
    {
      return false;
    }

  while( 1 )
    {
      fp >> tempstr;
      if( "STATION" == tempstr )
	{
	  break;
	}
    }
  fp >> tempstr;
  fp >> tempint;
  config_adc.station = tempint;

  while( 1 )
    {
      fp >> tempstr;
      if( "PED" == tempstr )
	{
	  break;
	}
    }

  while( tempint >= 0 )
    {
      fp >> tempint;

      if ( 0 == tempint )
	{
	  short ctrl[3];

	  for (int i = 0; i < 3; ++i)
	    {
	      fp >> ( ctrl[ i ] );
	    }
	  config_adc.Ctrl = ( (ctrl[0] << 2) | (ctrl[1] << 1) | (ctrl[0]) );
	}
      else if( tempint > 0 && tempint <= 16 )
	{
	  for (int i = 0; i < 3 ; ++i)
	    {
	      fp >> ( config_adc.Para[ tempint-1 ][ i ] );
	    }
	}
    }

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
      if ( 1e4 < ( now - start ) )
	{
	  start = clock();
	  pMan->pDisplay->form( pMan->pADC , pMan->lock_isConfiged);

	  pMan->lock_isConfiged = false;
	}

    }

}


void* CManager::daqThread( void* args )
{
  CManager* pMan = ( CManager* )args;

  pMan->lock_isDaqQuited = false;

  pMan->daqCycle();
  pMan->lock_isStarted = false;

  pMan->lock_isDaqQuited = true;
}



