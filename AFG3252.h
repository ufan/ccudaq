#ifndef AFG3252_H
#define AFG3252_H
#include <string>
#include "VISAInstrument.h"

class AFG3252 : public VISAInstrument
{
public:
    AFG3252();
    AFG3252(const char*name,const char* IP);
    ~AFG3252();

private:
    //copy control, declared but not implemented,users should not copy an VISAInstrument
    AFG3252(const AFG3252&);
    AFG3252& operator =(const AFG3252& rhs);

public:
    void SetIP(const char* ip);
    std::string GetIP();

    //output setting
    bool Reset();//load default settings
    bool PowerOn(int channel);
    bool PowerOff(int channel);
    bool SetImpedance(int channel,int impedance);
    bool SetHighZ(int channel);
    bool SetNormalZ(int channel);
    bool SetPolarity(int channel, bool isNormal=true);
    bool IsNormalPolarity(int channel);
    bool IsInversePolarity(int channel);

    //frequency
    bool SetFrequency(int channel,int frq,const char* unit="Hz");
    bool GetFrequency(int channel,char* response);
    bool TurnOnFrqConcurrent(int channel=1);
    bool TurnOffFrqConcurrent(int channel=1);
    bool IsFrqConcurrent();
    bool PhaseInitiate();

    //voltage level
    bool TurnOnVoltConcurrent(int channel=1);
    bool TurnOffVoltConcurrent(int channel=1);
    bool IsVoltConcurrent();
    bool SetVoltageHigh(int channel,float voltage,const char* unit="V");
    bool SetVoltageLow(int channel,float voltage,const char* unit="V");
    bool GetVoltageHigh(int channel,char* response);
    bool GetVoltageLow(int channel,char* response);

    //pulse
    bool SetShape(int channel,const char* shape="pulse");
    bool SetPulseWidth(int channel,float width,const char* unit="ns");
    bool GetPulseWidth(int channel,char* response);
    bool SetPulseDCycle(int channel,float percent);
    bool GetPulseDCycle(int channel,char* response);
    bool SetPulseHold(int channel,bool isWidth=true);//true for width,false for dcycle
    bool SetPulseDelay(int channel,int delay,const char* unit="ns");
    bool GetPulseDelay(int channel,char* response);
    bool SetPulsePeriod(int channel,float period,const char* unit="ns");
    bool GetPulsePeriod(int channel,char* response);
    bool SetPulseLeadingEdge(int channel,float edge,const char* unit="ns");
    bool GetPulseLeadingEdge(int channel,char* response);
    bool SetPulseTrailingEdge(int channel,float edge,const char* unit="ns");
    bool GetPulseTrailingEdge(int channel,char* response);

private:
    std::string IPaddr;
};

#endif // AFG3252_H
