#include <string.h>
#include "SYX527.h"
#include <windows.h>

using namespace std;
/////////SYX527_Module/////////////////////////////
SYX527_Module::SYX527_Module(SYX527 *controller, ushort slot_num, HVChannels &channels):
    fCrate(controller),fSlot(slot_num),fChNum(channels.size()),
    fVSet(0),fISet(25.0),fRUp(30.0),fRDWn(30.0)
{
    //
    fChList.resize(fChNum);
    fPChList=&fChList[0];

    fChSetName.resize(fChNum);

    for(int i=0;i<fChNum;i++){
        fChList[i]=channels[i].ch_id;
        fChSetName[i]=channels[i].ch_name;
    }
    //
    fState.resize(fChNum);
    fPState=&fState[0];

    fV0Set.resize(fChNum);
    fPV0Set=&fV0Set[0];

    fI0Set.resize(fChNum);
    fPI0Set=&fI0Set[0];

    fVMon.resize(fChNum);
    fPVMon=&fVMon[0];

    fIMon.resize(fChNum);
    fPIMon=&fIMon[0];

    fPChName=new char[fChNum][MAX_CH_NAME];
}

SYX527_Module::~SYX527_Module()
{
    delete []fPChName;
}

bool SYX527_Module::updateChName()
{
    //update
    if(!fCrate->connect())
        return false;

    for(int i=0;i<fChNum;i++){
        if(!fCrate->setChName(fSlot,fChList[i],fChSetName[i].c_str())){
            fCrate->disConnect();
            return false;
        }
    }

    Sleep(1000);
    //confirm
    if(!fCrate->getChName(fSlot,fChNum,fPChList,fPChName)){
        fCrate->disConnect();
        return false;
    }
    else{
        string tempstr;
        for(int i=0;i<fChNum;i++){
            tempstr=reinterpret_cast<char*>(fPChName[i]);
            if(tempstr != fChSetName[i]){
                fCrate->disConnect();
                return false;
            }
        }
    }

    fCrate->disConnect();
    return true;
}

bool SYX527_Module::updateVSet()
{
    //update
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->SetV0(fSlot,fChNum,fPChList,fVSet);
    fCrate->disConnect();

    return flag;
}

bool SYX527_Module::updateISet()
{
    //update
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->SetI0(fSlot,fChNum,fPChList,fISet);
    fCrate->disConnect();

    return flag;
}


bool SYX527_Module::updateRampUp()
{
    //update
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->SetRampUp(fSlot,fChNum,fPChList,fRUp);
    fCrate->disConnect();

    return flag;
}

bool SYX527_Module::updateRampDown()
{
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->SetRampDown(fSlot,fChNum,fPChList,fRDWn);
    fCrate->disConnect();

    return flag;
}

bool SYX527_Module::getState()
{
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->GetState(fSlot,fChNum,fPChList,fPState);
    fCrate->disConnect();

    return flag;
}

bool SYX527_Module::getV0Set()
{
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->GetV0(fSlot,fChNum,fPChList,fPV0Set);
    fCrate->disConnect();

    return flag;
}

bool SYX527_Module::getI0Set()
{
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->GetI0(fSlot,fChNum,fPChList,fPI0Set);
    fCrate->disConnect();

    return flag;
}

bool SYX527_Module::getVMon()
{
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->GetVMon(fSlot,fChNum,fPChList,fPVMon);
    fCrate->disConnect();

    return flag;
}

bool SYX527_Module::getIMon()
{
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->GetIMon(fSlot,fChNum,fPChList,fPIMon);
    fCrate->disConnect();

    return flag;
}

bool SYX527_Module::getChName()
{
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->getChName(fSlot,fChNum,fPChList,fPChName);
    fCrate->disConnect();

    return flag;
}

bool SYX527_Module::PowerOn()
{
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->TurnOn(fSlot,fChNum,fPChList);
    fCrate->disConnect();

    return flag;
}

bool SYX527_Module::PowerOff()
{
    if(!fCrate->connect())
        return false;

    bool flag=fCrate->TurnOff(fSlot,fChNum,fPChList);
    fCrate->disConnect();

    return flag;
}

bool SYX527_Module::update(HVChannels &channels)
{
    if(!fCrate->connect())
        return false;
    else{
        if(!fCrate->GetState(fSlot,fChNum,fPChList,fPState)){
            fCrate->disConnect();
            return false;
        }
        if(!fCrate->GetV0(fSlot,fChNum,fPChList,fPV0Set)){
            fCrate->disConnect();
            return false;
        }
        if(!fCrate->GetI0(fSlot,fChNum,fPChList,fPI0Set)){
            fCrate->disConnect();
            return false;
        }
        if(!fCrate->GetVMon(fSlot,fChNum,fPChList,fPVMon)){
            fCrate->disConnect();
            return false;
        }
        if(!fCrate->GetIMon(fSlot,fChNum,fPChList,fPIMon)){
            fCrate->disConnect();
            return false;
        }
        if(!fCrate->getChName(fSlot,fChNum,fPChList,fPChName)){
            fCrate->disConnect();
            return false;
        }

        if(channels.size() != fChNum){
            channels.resize(fChNum);
        }
        for(int i=0;i<fChNum;i++){
            channels[i].slot=fSlot;
            channels[i].ch_id=fChList[i];
            strncpy(channels[i].ch_name,reinterpret_cast<char*>(fPChName+i),MAX_CH_NAME);
            channels[i].V0Set=fV0Set[i];
            channels[i].I0Set=fI0Set[i];
            channels[i].VMon=fVMon[i];
            channels[i].IMon=fIMon[i];
            channels[i].state=static_cast<bool>(fState[i]);
        }

    }

    fCrate->disConnect();
    return true;
}

//////////SYX527///////////////////////////////////
SYX527::SYX527():
    fStatus(CAENHV_OK)
{
    pthread_mutex_init(&f_lock,NULL);
}

SYX527::SYX527(const char *IP, const char *usrName, const char *pssWord):
    fIPAddr(IP),fUserName(usrName),fPassWord(pssWord),fHandle(0),fStatus(CAENHV_OK)
{
    pthread_mutex_init(&f_lock,NULL);
}

SYX527::~SYX527()
{
    pthread_mutex_destroy(&f_lock);
}

bool SYX527::status()
{
    if(fStatus == CAENHV_OK)
        return true;
    else
        return false;
}

string SYX527::getErrorDesc()
{
    return fErrorDesc;
}

bool SYX527::connect()
{
    pthread_mutex_lock(&f_lock);
    char temp[50];
    strncpy(temp,fIPAddr.c_str(),50);
    fStatus=CAENHV_InitSystem(SY1527,LINKTYPE_TCPIP,temp,
                              fUserName.c_str(),fPassWord.c_str(),&fHandle);
    if(fStatus != CAENHV_OK){
        fErrorDesc="Can't connect SYX527";
        pthread_mutex_unlock(&f_lock);
        return false;
    }
    else
        return true;
}

bool SYX527::disConnect()
{
    fStatus=CAENHV_DeinitSystem(fHandle);

    if(fStatus != CAENHV_OK){
        fErrorDesc="Can't disconnect SYX527";
        pthread_mutex_unlock(&f_lock);
        return false;
    }
    else{
        pthread_mutex_unlock(&f_lock);
        return true;
    }
}

bool SYX527::setChName(unsigned short slot, unsigned short chnum, const unsigned short *chlist, const char *chname)
{
    fStatus=CAENHV_SetChName(fHandle,slot,chnum,chlist,chname);
    if(fStatus != CAENHV_OK){
        fErrorDesc=CAENHV_GetError(fHandle);
        return false;
    }
    else
        return true;
}

bool SYX527::setChName(unsigned short slot, unsigned short ch_id, const char *chname)
{
    fStatus=CAENHV_SetChName(fHandle,slot,1,&ch_id,chname);
    if(fStatus != CAENHV_OK){
        fErrorDesc=CAENHV_GetError(fHandle);
        return false;
    }
    else
        return true;
}

bool SYX527::setChName(HVChannels& channels)
{
    HVChannels::iterator it;
    for(it=channels.begin();it!=channels.end();it++){
        if(!setChName(*it))
            return false;
    }
    return true;
}

bool SYX527::setChName(const HVChannel &channel)
{
    fStatus=CAENHV_SetChName(fHandle,channel.slot,1,&(channel.ch_id),channel.ch_name);
    if(fStatus != CAENHV_OK){
        fErrorDesc=CAENHV_GetError(fHandle);
        return false;
    }
    else
        return true;
}

bool SYX527::getChName(unsigned short slot, unsigned short ch_num, const unsigned short *chlist, char (*chname)[MAX_CH_NAME])
{
    fStatus=CAENHV_GetChName(fHandle,slot,ch_num,chlist,chname);
    if(fStatus != CAENHV_OK){
        fErrorDesc=CAENHV_GetError(fHandle);
        return false;
    }
    else
        return true;
}

bool SYX527::getChName(unsigned short slot, unsigned short ch_id, char *chname)
{
    return getChName(slot,1,&ch_id,reinterpret_cast<char(*)[MAX_CH_NAME]>(chname));
}

bool SYX527::getChName(HVChannel &channel)
{
    return getChName(channel.slot,channel.ch_id,channel.ch_name);
}

bool SYX527::getChName(HVChannels &channels)
{
    HVChannels::iterator it;
    for(it=channels.begin();it!=channels.end();it++){
        if(!getChName(*it))
            return false;
    }
    return true;
}

bool SYX527::setChParam(unsigned short slot, const char *param, unsigned short chnum, const unsigned short *chlist, void *parvalue)
{
    fStatus=CAENHV_SetChParam(fHandle,slot,param,chnum,chlist,parvalue);
    if(fStatus != CAENHV_OK){
        fErrorDesc=CAENHV_GetError(fHandle);
        return false;
    }
    else
        return true;
}

bool SYX527::getChParam(unsigned short slot, const char *param, unsigned short chnum, const unsigned short *chlist, void *parvallist)
{
    fStatus=CAENHV_GetChParam(fHandle,slot,param,chnum,chlist,parvallist);
    if(fStatus != CAENHV_OK){
        fErrorDesc=CAENHV_GetError(fHandle);
        return false;
    }
    else
        return true;
}

bool SYX527::TurnOn(unsigned short slot, unsigned short chnum, const unsigned short *chlist)
{
    ulong Pw=1;
    return setChParam(slot,"Pw",chnum,chlist,&Pw);
}

bool SYX527::TurnOff(unsigned short slot, unsigned short chnum, const unsigned short *chlist)
{
    ulong Pw=0;
    return setChParam(slot,"Pw",chnum,chlist,&Pw);
}

bool SYX527::SetRampUp(unsigned short slot, unsigned short chnum, const unsigned short *chlist, float RUp)
{
    return setChParam(slot,"RUp",chnum,chlist,&RUp);
}

bool SYX527::SetRampDown(unsigned short slot, unsigned short chnum, const unsigned short *chlist, float RDwn)
{
    return setChParam(slot,"RDWn",chnum,chlist,&RDwn);
}

bool SYX527::SetV0(unsigned short slot, unsigned short chnum, const unsigned short *chlist, float V)
{
    return setChParam(slot,"V0Set",chnum,chlist,&V);
}

bool SYX527::SetI0(unsigned short slot, unsigned short chnum, const unsigned short *chlist, float I)
{
    return setChParam(slot,"I0Set",chnum,chlist,&I);
}

bool SYX527::GetState(unsigned short slot, unsigned short chnum, const unsigned short *chlist, ulong *parvallist)
{
    return getChParam(slot,"Pw",chnum,chlist,parvallist);
}

bool SYX527::GetV0(unsigned short slot, unsigned short chnum, const unsigned short *chlist, float *parvallist)
{
    return getChParam(slot,"V0Set",chnum,chlist,parvallist);
}

bool SYX527::GetI0(unsigned short slot, unsigned short chnum, const unsigned short *chlist, float *parvallist)
{
    return getChParam(slot,"I0Set",chnum,chlist,parvallist);
}

bool SYX527::GetVMon(unsigned short slot, unsigned short chnum, const unsigned short *chlist, float *parvallist)
{
    return getChParam(slot,"VMon",chnum,chlist,parvallist);
}

bool SYX527::GetIMon(unsigned short slot, unsigned short chnum, const unsigned short *chlist, float *parvallist)
{
    return getChParam(slot,"IMon",chnum,chlist,parvallist);
}

bool SYX527::GetRampUp(unsigned short slot, unsigned short chnum, const unsigned short *chlist, float *parvallist)
{
    return getChParam(slot,"RUp",chnum,chlist,parvallist);
}

bool SYX527::GetRampDown(unsigned short slot, unsigned short chnum, const unsigned short *chlist, float *parvallist)
{
    return getChParam(slot,"RDWn",chnum,chlist,parvallist);
}
