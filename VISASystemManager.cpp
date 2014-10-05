#include "VISASystemManager.h"
#include "VISAInstrument.h"
//#include "stdio.h"

VISASystemManager* VISASystemManager::fInstance = 0;
ViSession VISASystemManager::fDefaultRM=0;
ViStatus VISASystemManager::fStatus=VI_STATE_UNKNOWN;

VISASystemManager::VISASystemManager()
{
    fStatus = viOpenDefaultRM(&fDefaultRM);
}

VISASystemManager::~VISASystemManager()
{
    //printf("destruct VISASystemManager\n");
    //static object will be destructed after main fucntion return.
    //while undeleted heap object will be deleted rudely by OS without
    //invoking destructor.
    Clean();
}
VISASystemManager* VISASystemManager::GetInstance()
{
    static VISASystemManager visamanager;
    if(!fInstance){
        fInstance=&visamanager;
    }
    return fInstance;
}

bool VISASystemManager::Register(VISAInstrument *instr)
{
    std::string deviceName=instr->GetName();
    std::pair<std::map<std::string,VISAInstrument*>::iterator,bool> ret;
    ret=GetInstance()->insert(std::pair<std::string,VISAInstrument*>(deviceName,instr));
    return ret.second;
}

void VISASystemManager::DeRegister(std::string deviceName)
{
    iterator it;
    VISASystemManager* self=GetInstance();
    it=self->find(deviceName);
    if(it!=self->end()){
        it->second->Close();
        self->erase(it);
    }
}

void VISASystemManager::Clean()
{
    iterator it;
    VISASystemManager* self=GetInstance();
    for(it=self->begin();it!=self->end();++it){
        it->second->Close();//just close the resource
                                        // delete will invoke Deregister(),which
                                       // in turn will make erase item,ultimately
                                       //make ++it invalid,a run time error
    }
    self->clear();
    //close visa resource manager
    viClose(fDefaultRM);
}

ViSession VISASystemManager::GetDefaultRM()
{
    return fDefaultRM;
}

ViStatus VISASystemManager::ReInit()
{
    fStatus=viOpenDefaultRM(&fDefaultRM);
    return fStatus;
}

bool VISASystemManager::Status()
{
    if(fStatus < VI_SUCCESS)
        return false;
    else
        return true;
}
