#include "AFG3252.h"
#include <stdio.h>
#include <string.h>

using namespace std;

AFG3252::AFG3252()
{

}

AFG3252::AFG3252(const char* name,const char* IP):
    IPaddr(IP)
{
    string rsrc="TCPIP0::";
    rsrc.append(IP);
    rsrc.append("::inst0::INSTR");
    SetResourceString(rsrc.c_str());
    SetName(name);
    Initialize();
}

AFG3252::~AFG3252()
{

}

void AFG3252::SetIP(const char *ip)
{
    IPaddr=ip;
    string rsrc="TCPIP0::";
    rsrc.append(IPaddr);
    rsrc.append("::inst0::INSTR");
    SetResourceString(rsrc.c_str());
}

string AFG3252::GetIP()
{
    return IPaddr;
}

bool AFG3252::Reset()
{
    char command[]="*RST";
    char error_desc[512];
    ViUInt32 retCnt;
    fStatus=viWrite(fViSession,(ViBuf)command,strlen(command)+1,&retCnt);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::PowerOff(int channel)
{
    char command[256];
    char error_desc[512];
    ViUInt32 retCnt;
    sprintf(command,"output%d:state off",channel);
    fStatus=viWrite(fViSession,(ViBuf)command,strlen(command)+1,&retCnt);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::PowerOn(int channel)
{
    char command[256];
    char error_desc[512];
    ViUInt32 retCnt;
    sprintf(command,"output%d:state on",channel);
    fStatus=viWrite(fViSession,(ViBuf)command,strlen(command)+1,&retCnt);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetImpedance(int channel, int impedance)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"output%d:impedance %dOhm\n",channel,impedance);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetHighZ(int channel)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"output%d:impedance maximum\n",channel);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetNormalZ(int channel)
{
    return SetImpedance(channel,50);
}

/*The arbitrary/function generator
//automatically changes to the Continuous mode if any waveform is selected other
//than sine, square, ramp, or an arbitrary waveform*/
bool AFG3252::SetShape(int channel, const char *shape)
{
    char command[256];
    char error_desc[512];
    ViUInt32 retCnt;
    sprintf(command,"source%d:function:shape %s",channel,shape);
    fStatus=viWrite(fViSession,(ViBuf)command,strlen(command)+1,&retCnt);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetPolarity(int channel, bool isNormal)
{
    char command[256];
    char error_desc[512];
    ViUInt32 retCnt;
    if(isNormal){
        sprintf(command,"output%d:polarity normal",channel);
    }
    else{
        sprintf(command,"output%d:polarity inverted",channel);
    }
    fStatus=viWrite(fViSession,(ViBuf)command,strlen(command)+1,&retCnt);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::IsNormalPolarity(int channel)
{
    char retvalue[10];
    char error_desc[512];
    fStatus=viQueryf(fViSession,"output%d:polarity?\n","%s",channel,retvalue);
    viStatusDesc(fViSession,fStatus,error_desc);
    fErrorCode=error_desc;

    string response=retvalue;
    if(response=="NORM")
        return true;
    else
        return false;
}

bool AFG3252::IsInversePolarity(int channel)
{
    char retvalue[10];
    char error_desc[512];
    fStatus=viQueryf(fViSession,"output%d:polarity?\n","%s",channel,retvalue);
    viStatusDesc(fViSession,fStatus,error_desc);
    fErrorCode=error_desc;

    string response=retvalue;
    if(response=="INV")
        return true;
    else
        return false;
}

bool AFG3252::SetFrequency(int channel, int frq, const char *unit)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:frequency:fixed %d%s\n",channel,frq,unit);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::GetFrequency(int channel, char *response)
{
    char error_desc[512];
    fStatus=viQueryf(fViSession,"source%d:frequency:fixed?\n","%s",channel,response);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::TurnOffFrqConcurrent(int channel)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:frequency:concurrent off\n",channel);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::TurnOnFrqConcurrent(int channel)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:frequency:concurrent on\n",channel);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::IsFrqConcurrent()
{
    int status;
    char error_desc[512];
    fStatus=viQueryf(fViSession,"source1:frequency:concurrent?\n","%d",&status);
    viStatusDesc(fViSession,fStatus,error_desc);
    fErrorCode=error_desc;

    if(status)
        return true;
    else
        return false;
}

bool AFG3252::PhaseInitiate()
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source1:phase:initiate\n");
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::TurnOnVoltConcurrent(int channel)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:voltage:concurrent:state on\n",channel);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::TurnOffVoltConcurrent(int channel)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:voltage:concurrent:state off\n",channel);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::IsVoltConcurrent()
{
    int status;
    char error_desc[512];
    fStatus=viQueryf(fViSession,"source1:voltage:concurrent:state?\n","%d",&status);
    viStatusDesc(fViSession,fStatus,error_desc);
    fErrorCode=error_desc;

    if(status)
        return true;
    else
        return false;
}

bool AFG3252::SetVoltageHigh(int channel, float voltage, const char *unit)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:voltage:level:immediate:high %.2f%s\n",channel,voltage,unit);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetVoltageLow(int channel, float voltage, const char *unit)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:voltage:level:immediate:low %.2f%s\n",channel,voltage,unit);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::GetVoltageHigh(int channel, char *response)
{
    char error_desc[512];
    fStatus=viQueryf(fViSession,"source%d:voltage:level:immediate:high?\n","%s",channel,response);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::GetVoltageLow(int channel, char *response)
{
    char error_desc[512];
    fStatus=viQueryf(fViSession,"source%d:voltage:level:immediate:low?\n","%s",channel,response);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetPulseWidth(int channel, float width, const char *unit)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:pulse:width %.2f%s\n",channel,width,unit);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::GetPulseWidth(int channel, char *response)
{
    char error_desc[512];
    fStatus=viQueryf(fViSession,"source%d:pulse:width?\n","%s",channel,response);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetPulseDCycle(int channel, float percent)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:pulse:dcycle %.3f\n",channel,percent);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::GetPulseDCycle(int channel, char *response)
{
    char error_desc[512];
    fStatus=viQueryf(fViSession,"source%d:pulse:dcycle?\n","%s",channel,response);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetPulseHold(int channel, bool isWidth)
{
    char error_desc[512];
    if(isWidth)
        fStatus=viPrintf(fViSession,"source%d:pulse:hold width\n",channel);
    else
        fStatus=viPrintf(fViSession,"source%d:pulse:hold duty\n",channel);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetPulseDelay(int channel, int delay, const char *unit)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:pulse:delay %d%s\n",channel,delay,unit);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::GetPulseDelay(int channel, char *response)
{
    char error_desc[512];
    fStatus=viQueryf(fViSession,"source%d:pulse:delay?\n","%s",channel,response);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetPulsePeriod(int channel, float period, const char *unit)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:pulse:period %.2f%s\n",channel,period,unit);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::GetPulsePeriod(int channel, char *response)
{
    char error_desc[512];
    fStatus=viQueryf(fViSession,"source%d:pulse:period?\n","%s",channel,response);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetPulseLeadingEdge(int channel, float edge, const char *unit)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:pulse:transition:leading %.1f%s\n",channel,edge,unit);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::GetPulseLeadingEdge(int channel, char *response)
{
    char error_desc[512];
    fStatus=viQueryf(fViSession,"source%d:pulse:transition:leading?\n","%s",channel,response);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::SetPulseTrailingEdge(int channel, float edge, const char *unit)
{
    char error_desc[512];
    fStatus=viPrintf(fViSession,"source%d:pulse:transition:trailing %.1f%s\n",channel,edge,unit);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}

bool AFG3252::GetPulseTrailingEdge(int channel, char *response)
{
    char error_desc[512];
    fStatus=viQueryf(fViSession,"source%d:pulse:transition:trailing?\n","%s",channel,response);
    if(fStatus<VI_SUCCESS){
        viStatusDesc(fViSession,fStatus,error_desc);
        fErrorCode=error_desc;
        return false;
    }
    return true;
}
