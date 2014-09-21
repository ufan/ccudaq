#include "config.h"
#include "../display.h"

/////////////////////////////
//CC_Config definition
/////////////////////////////
CC_Config::CC_Config():
    GlobalMode(0),Delays(0),ScalReadCtrl(0),
    SelectLED(0),SelectNIMO(0),SelectUserDevice(0),
    TimingDGGA(0),TimingDGGB(0),ExtendedDelay(0),
    LAMMask(0),UsbBufferSetup(0)
{

}

CC_Config::CC_Config(const CC_Config &rhs)
{
    GlobalMode=rhs.GlobalMode;
    Delays=rhs.Delays;
    ScalReadCtrl=rhs.ScalReadCtrl;
    SelectLED=rhs.SelectLED;
    SelectNIMO=rhs.SelectNIMO;
    SelectUserDevice=rhs.SelectUserDevice;
    TimingDGGA=rhs.TimingDGGA;
    TimingDGGB=rhs.TimingDGGB;
    ExtendedDelay=rhs.ExtendedDelay;
    LAMMask=rhs.LAMMask;
    UsbBufferSetup=rhs.UsbBufferSetup;
}

CC_Config::~CC_Config()
{

}

CC_Config&
CC_Config::operator=(const CC_Config& rhs)
{
    GlobalMode=rhs.GlobalMode;
    Delays=rhs.Delays;
    ScalReadCtrl=rhs.ScalReadCtrl;
    SelectLED=rhs.SelectLED;
    SelectNIMO=rhs.SelectNIMO;
    SelectUserDevice=rhs.SelectUserDevice;
    TimingDGGA=rhs.TimingDGGA;
    TimingDGGB=rhs.TimingDGGB;
    ExtendedDelay=rhs.ExtendedDelay;
    LAMMask=rhs.LAMMask;
    UsbBufferSetup=rhs.UsbBufferSetup;

    return *this;
}

bool
CC_Config::operator ==(const CC_Config& lhs,const CC_Config& rhs)
{
    if(lhs.GlobalMode != rhs.GlobalMode)        return false;
    if(lhs.Delays != rhs.Delays)                return false;
    if(lhs.ScalReadCtrl != rhs.ScalReadCtrl)    return false;
    if(lhs.SelectLED != rhs.SelectLED)          return false;
    if(lhs.SelectNIMO != rhs.SelectNIMO)        return false;
    if(lhs.SelectUserDevice != rhs.SelectUserDevice) return false;
    if(lhs.TimingDGGA != rhs.TimingDGGA)        return false;
    if(lhs.TimingDGGB != rhs.TimingDGGB)        return false;
    if(lhs.ExtendedDelay != rhs.ExtendedDelay)  return false;
    if(lhs.LAMMask != rhs.LAMMask)              return false;
    if(lhs.UsbBufferSetup != rhs.UsbBufferSetup)    return false;

    return true;
}

void
CC_Config::dump()
{

}

void
CC_Config::dump(CDisplay* pDisplay)
{

}

void
CC_Config::dump(std::string& tempstr)
{

}

////////////////////////////////////
//Module_Config definition
////////////////////////////////////
Module_Config::Module_Config()
{

}

Module_Config::Module_Config(const Module_Config &rhs)
{
    Ctrl=rhs.Ctrl;
    station=rhs.station;
    name=rhs.name;
    for(int i=0;i<16;i++){
        UT[i]=rhs.UT[i];
        LT[i]=rhs.LT[i];
        PED[i]=rhs.PED[i];
    }
}

Module_Config::~Module_Config()
{

}

Module_Config&
Module_Config::operator =(const Module_Config& rhs)
{
    Ctrl=rhs.Ctrl;
    station=rhs.station;
    name=rhs.name;
    for(int i=0;i<16;i++){
        UT[i]=rhs.UT[i];
        LT[i]=rhs.LT[i];
        PED[i]=rhs.PED[i];
    }

    return *this;
}

bool
Module_Config::operator ==(const Module_Config& lhs,const Module_Config& rhs)
{
    if(lhs.Ctrl != rhs.Ctrl)        return false;
    if(lhs.station != rhs.station)  return false;
    if(lhs.name != rhs.name)        return false;
    for(int i=0;i<16;i++){
        if(lhs.LT[i] != rhs.LT[i])  return false;
        if(lhs.UT[i] != rhs.UT[i])  return false;
        if(lhs.PED[i] != rhs.PED[i])  return false;
    }

    return true;
}

void
Module_Config::dump()
{

}

void
Module_Config::dump(CDisplay* pDisplay)
{

}

void
Module_Config::dump(std::string& tempstr)
{

}
