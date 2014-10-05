#ifndef VISASYSTEMMANAGER_H
#define VISASYSTEMMANAGER_H
#include <string>
#include <map>
#include "visa.h"
class VISAInstrument;

class VISASystemManager : public std::map<std::string,VISAInstrument*>
{
public:
    ~VISASystemManager();
    static VISASystemManager* GetInstance();
    static bool Register(VISAInstrument* instr);
    static void DeRegister(std::string deviceName);
    static void Clean();
    static ViSession GetDefaultRM();
    static ViStatus ReInit();

    static bool Status();
private:
    VISASystemManager();

private:
    static VISASystemManager* fInstance;
    static ViSession fDefaultRM;
    static ViStatus  fStatus;//initialize=VI_STATE_UNKNOWN(-1)
};

#endif // VISASYSTEMMANAGER_H
