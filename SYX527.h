#ifndef SYX527_H
#define SYX527_H

#include <string>
#include <vector>
#include <map>
#include "CAENHVWrapper.h"

struct HVChannel
{
    //in
    ushort slot;
    ushort ch_id;
    char ch_name[MAX_CH_NAME];
    //out
    float V0Set;
    float I0Set;
    float VMon;
    float IMon;
    bool state;
};
typedef std::vector<HVChannel> HVChannels;
typedef std::map<ushort,HVChannels> HVGroup;

class SYX527
{
public:
    SYX527();
    SYX527(const char* IP,const char* usrName,const char* pssWord);
    ~SYX527();

public:
    inline void setIP(const char* ipAddr) {fIPAddr=ipAddr;}
    inline void setUsername(const char* name) {fUserName=name;}
    inline void setPassword(const char* psswd) {fPassWord=psswd;}
    inline std::string getIP() {return fIPAddr;}
    inline std::string getUsername() {return fUserName;}
    inline std::string getPassword() {return fPassWord;}
    //status
    bool status();
    std::string getErrorDesc();
    //conncetion
    bool connect();
    bool disConnect();
    //utility
    bool setChName(ushort slot,ushort ch_id,const char* chname);
    bool setChName(ushort slot,ushort chnum,const ushort *chlist,const char* chname);
    bool setChName(const HVChannel& channel);
    bool setChName(HVChannels& channels);
    bool getChName(ushort slot,ushort ch_id,char* chname);
    bool getChName(ushort slot,ushort ch_num,const ushort *chlist,char (*chname)[MAX_CH_NAME]);
    bool getChName(HVChannel& channel);
    bool getChName(HVChannels& channels);//channels must be in the same slot

    bool setChParam(ushort slot,const char* param,ushort chnum,const ushort *chlist,void* parvalue);
    bool getChParam(ushort slot,const char* param,ushort chnum,const ushort *chlist,void* parvallist);

    //usful functions
    bool TurnOn(ushort slot,ushort chnum,const ushort *chlist);
    bool TurnOff(ushort slot,ushort chnum,const ushort *chlist);
    bool SetRampUp(ushort slot,ushort chnum,const ushort *chlist,float RUp);
    bool SetRampDown(ushort slot,ushort chnum,const ushort *chlist,float RDwn);
    bool SetV0(ushort slot,ushort chnum,const ushort *chlist,float V);
    bool SetI0(ushort slot,ushort chnum,const ushort *chlist,float I);

    bool GetState(ushort slot,ushort chnum,const ushort *chlist,ulong* parvallist);
    bool GetV0(ushort slot,ushort chnum,const ushort *chlist,float* parvallist);
    bool GetI0(ushort slot,ushort chnum,const ushort *chlist,float* parvallist);
    bool GetVMon(ushort slot,ushort chnum,const ushort *chlist,float* parvallist);
    bool GetIMon(ushort slot,ushort chnum,const ushort *chlist,float* parvallist);
    bool GetRampUp(ushort slot,ushort chnum,const ushort *chlist,float* parvallist);
    bool GetRampDown(ushort slot,ushort chnum,const ushort *chlist,float* parvallist);

private:
    //config
    std::string fIPAddr;
    std::string fUserName;
    std::string fPassWord;
    int fHandle;
    //status
    CAENHVRESULT fStatus;
    std::string fErrorDesc;

};


class SYX527_Module
{
public:
    SYX527_Module(SYX527 *controller,ushort slot,HVChannels& channels);
    ~SYX527_Module();

public:
    inline void setVSet(float vset) {fVSet=vset;}
    inline void setISet(float iset) {fISet=iset;}
    inline void setRampUp(float rup) {fRUp=rup;}
    inline void setRampDown(float rdown) {fRDWn=rdown;}
    inline ushort getSlot() const {return fSlot;}

    bool updateChName();
    bool updateVSet();
    bool updateISet();
    bool updateRampUp();
    bool updateRampDown();

    bool getState();
    bool getV0Set();
    bool getI0Set();
    bool getVMon();
    bool getIMon();
    bool getChName();

    bool update(HVChannels& channels);
    bool PowerOn();
    bool PowerOff();
private:
    SYX527 *fCrate;
    ushort fSlot;
    ushort fChNum;
    //ushort *fChList;
    std::vector<ushort> fChList;
    ushort* fPChList;
    //std::string *fChSetName;
    std::vector<std::string> fChSetName;

    //input
    float fVSet;
    float fISet;
    //ushort fPw;
    float fRUp;
    float fRDWn;

    //output
    //ushort* fState;
    std::vector<ulong> fState;
    ulong* fPState;
    //float *fV0Set;
    std::vector<float> fV0Set;
    float* fPV0Set;
    //float *fI0Set;
    std::vector<float> fI0Set;
    float* fPI0Set;
    //float *fVMon;
    std::vector<float> fVMon;
    float* fPVMon;
    //float *fIMon;
    std::vector<float> fIMon;
    float* fPIMon;
    char (*fPChName)[MAX_CH_NAME];

};

#endif // SYX527_H
