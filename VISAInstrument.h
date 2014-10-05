#ifndef VISAInstrument_H
#define VISAInstrument_H

#include <string>
#include "visa.h"

class VISAInstrument
{
public:
    VISAInstrument();
    VISAInstrument(const char* deviceName,const char* rsrcName,ViAccessMode accessMode=VI_NULL,ViUInt32 openTimeout=VI_NULL);
    virtual ~VISAInstrument();

private:
    //copy control, declared but not implemented,users should not copy an VISAInstrument
    VISAInstrument(const VISAInstrument& rhs);
    VISAInstrument& operator=(const VISAInstrument& rhs);

public:
    inline void SetName(const char* name)
    { fDeviceName = name; }
    inline std::string GetName()
    { return fDeviceName; }
    inline void SetResourceString(const char* rcname)
    { fRsrcName=rcname; }
    inline std::string GetResourceString()
    { return fRsrcName; }
    inline void SetDefaultRM();
    inline std::string ErrorCode()
    { return fErrorCode; }
    inline std::string GetRsrcDesc()
    { return fRsrcDesc; }
    bool Status();
    bool Initialize();
    void Close();

protected:
    std::string fErrorCode;
    ViSession   fViSession;//ViSession
    ViStatus    fStatus;

private:
    std::string fDeviceName;//Unique Name idetify the intrument
    std::string fRsrcName;//Rsrc string
    std::string fRsrcDesc;
    ViSession   fDefaultRM;
    ViAccessMode fAccessMode;
    ViUInt32     fOpenTimeout;
};

#endif // VISAInstrument_H
