#include <sstream>
#include "nsclmodul.h"
#include "CCCUSB.h"

using namespace std;

NSCLmodule::NSCLmodule(CCCUSB *ccusb)
{
    mName="invalid module";
    mStation=-1;
    mCcu=ccusb;
}

NSCLmodule::NSCLmodule(CCCUSB *ccusb, int value):
{
    stringstream ss;
    ss << "unamed_station_" <<value;
    mName=ss.str();
    mStation=value;
    mCcu=ccusb;
}

NSCLmodule::NSCLmodule(CCCUSB *ccusb, int value, std::string name)
{
    mStation=value;
    mName=name;
    mCcu=ccusb;
}


void
NSCLmodule::setStation(int value)
{
    mStation=value;
}

void
NSCLmodule::setName(std::string name)
{
    mName=name;
}

bool
NSCLmodule::config(Module_Config &configIn)
{
    setName(configIn.getName());
    setStation(configIn.getStation());

    if(!setCtrlReg(configIn.getCtrl()))     return false;
    for(int i=0;i<16;i++){
        if(!setLT(i+1,configIn.getLT(i+1))) return false;
        if(!setUT(i+1,configIn.getUT(i+1))) return false;
        if(!setPED(i+1,configIn.getPED(i+1)))   return false;
    }

    return true;
}

Module_Config
NSCLmodule::getConfig()
{
    Module_Config moc;
    uint16_t feedback;

    moc.setName(mName);
    moc.setStation(mStation);

    getCtrlReg(feedback);moc.setCtrl(feedback);
    for(int i=0;i<16;i++){
        getLT(i+1,feedback);moc.setLT(i+1,feedback);
        getUT(i+1,feedback);moc.setUT(i+1,feedback);
        getPED(i+1,feedback);moc.setPED(i+1,feedback);
    }

    return moc;
}

bool
NSCLmodule::getCtrlReg(uint16_t &value)
{
    uint16_t qx;
    int status;

    status=mCcu->simpleRead16(mStation,0,6,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::clrCtrlReg()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,0,11,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::setCtrlReg(uint16_t value)
{
    uint16_t qx;
    int status;

    status=mCcu->simpleWrite16(mStation,0,19,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::enableUT()
{
    uint16_t data;
    getCtrlReg(data);
    data=data | 4;
    return setCtrlReg(data);
}

bool
NSCLmodule::enableLT()
{
    uint16_t data;
    getCtrlReg(data);
    data=data | 2;
    return setCtrlReg(data);
}

bool
NSCLmodule::enablePED()
{
    uint16_t data;
    getCtrlReg(data);
    data=data | 1;
    return setCtrlReg(data);
}

bool
NSCLmodule::disableUT()
{
    uint16_t qx;
    int status;
    uint16_t value=4;

    status=mCcu->simpleWrite16(mStation,0,23,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::disableLT()
{
    uint16_t qx;
    int status;
    uint16_t value=2;

    status=mCcu->simpleWrite16(mStation,0,23,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::disablePED()
{
    uint16_t qx;
    int status;
    uint16_t value=1;

    status=mCcu->simpleWrite16(mStation,0,23,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::setUT(int ch_id, uint16_t value)
{
    if(!slctUT())   return false;
    if(!setParaReg(ch_id-1,value))  return false;

    return true;
}

bool
NSCLmodule::setLT(int ch_id, uint16_t value)
{
    if(!slctLT())   return false;
    if(!setParaReg(ch_id-1,value))  return false;

    return true;
}

bool
NSCLmodule::setPED(int ch_id, uint16_t value)
{
    if(!slctPED())   return false;
    if(!setParaReg(ch_id-1,value))  return false;

    return true;
}

bool
NSCLmodule::getUT(int ch_id, uint16_t &value)
{
    if(!slctUT())   return false;
    if(!getParaReg(ch_id-1,value))  return false;

    return true;
}

bool
NSCLmodule::getLT(int ch_id, uint16_t &value)
{
    if(!slctLT())   return false;
    if(!getParaReg(ch_id-1,value))  return false;

    return true;
}

bool
NSCLmodule::getPED(int ch_id, uint16_t &value)
{
    if(!slctPED())   return false;
    if(!getParaReg(ch_id-1,value))  return false;

    return true;
}

bool
NSCLmodule::enableLam()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,0,26,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::disableLam()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,0,24,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::testLam()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,0,8,qx);

    if(status<0 || (qx != 0x3))   return false;
    return true;
}

bool
NSCLmodule::clrLam()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,0,10,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::getHitReg(uint16_t &value)
{
    uint16_t qx;
    int status;

    status=mCcu->simpleRead16(mStation,1,6,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::clrHitReg()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,1,11,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::clrDataMem()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,3,11,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::reset()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,0,9,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::readDataMem(int a, uint16_t &value)
{
    uint16_t qx;
    int status;

    status=mCcu->simpleRead16(mStation,a,0,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::writeDataMem(int a, uint16_t value)
{
    uint16_t qx;
    int status;

    status=mCcu->simpleWrite16(mStation,a,16,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::readSparse(int a, uint16_t &value)
{
    uint16_t qx;
    int status;

    status=mCcu->simpleRead16(mStation,0,4,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::setTestReg(int flag)
{
    if(!slctTest())     return false;
    if(flag<0 || flag>3){
        std::string msg="invalid Test register flag,valid range is 0-3";
        throw msg;
    }
    if(!setParaReg(flag,0)) return false;

    return true;
}

bool
NSCLmodule::clrTestReg()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,2,11,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::initTest()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,0,25,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::initCalib1()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,1,25,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::initCalib2()
{
    uint16_t qx;
    int status;

    status=mCcu->simpleControl(mStation,2,25,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::slctPED()
{
    uint16_t qx;
    int status;
    uint16_t value=0;

    status=mCcu->simpleWrite16(mStation,0,17,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::slctLT()
{
    uint16_t qx;
    int status;
    uint16_t value=0;

    status=mCcu->simpleWrite16(mStation,1,17,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::slctUT()
{
    uint16_t qx;
    int status;
    uint16_t value=0;

    status=mCcu->simpleWrite16(mStation,2,17,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::slctTest()
{
    uint16_t qx;
    int status;
    uint16_t value=0;

    status=mCcu->simpleWrite16(mStation,4,17,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::setParaReg(int a, uint16_t data)
{
    uint16_t qx;
    int status;

    status=mCcu->simpleWrite16(mStation,a,20,data,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

bool
NSCLmodule::getParaReg(int a, uint16_t &value)
{
    uint16_t qx;
    int status;

    status=mCcu->simpleRead16(mStation,a,1,value,qx);

    if(status<0 || !(qx&0x2))   return false;
    return true;
}

