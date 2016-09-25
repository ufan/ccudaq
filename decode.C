#include "stdio.h"
// #include <stdint.h>
#include <iostream>
#include <fstream>
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TList.h"
#include "TCollection.h"
#include "TString.h"
#include "TSystemFile.h"
#include "TSystemDirectory.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TSpectrum.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include <vector>
#include <map>
#include <string>

class Module_Config
{
public:
    Module_Config(){}
    Module_Config(const Module_Config& rhs){
      Ctrl=rhs.Ctrl;
      station=rhs.station;
      name=rhs.name;
      for(int i=0;i<16;i++){
        UT[i]=rhs.UT[i];
        LT[i]=rhs.LT[i];
        PED[i]=rhs.PED[i];
      }
    }
    ~Module_Config(){}

    Module_Config& operator=(const Module_Config& rhs)
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
    bool operator==(const Module_Config& rhs)
  {
    if(Ctrl != rhs.Ctrl)        return false;
    if(station != rhs.station)  return false;
    if(name != rhs.name)        return false;
    for(int i=0;i<16;i++){
      if(LT[i] != rhs.LT[i])  return false;
      if(UT[i] != rhs.UT[i])  return false;
      if(PED[i] != rhs.PED[i])  return false;
    }

    return true;
  }

private:
  UShort_t UT[16];
  UShort_t LT[16];
  Short_t PED[16];
  UShort_t Ctrl;
  int station;
  std::string name;

public:
  void dump(){}
  void dump(std::string& tempstr){}

  inline void setName(std::string value){
      name=value;
  }
  inline void setName(const char* value){
      name=value;
  }
  inline std::string getName(){
      return name;
  }
  inline void setStation(int value){
      station=value;
  }
  inline int getStation(){
      return station;
  }
  inline void setCtrl(UShort_t value){
      Ctrl=value;
  }
  inline UShort_t getCtrl(){
      return Ctrl;
  }
  inline void setLT(int ch_id,UShort_t value){
      LT[ch_id-1]=value;
  }
  inline UShort_t getLT(int ch_id){
      return LT[ch_id-1];
  }
  inline void setUT(int ch_id,UShort_t value){
      UT[ch_id-1]=value;
  }
  inline UShort_t getUT(int ch_id){
      return UT[ch_id-1];
  }
  inline void setPED(int ch_id,Short_t value){
      PED[ch_id-1]=value;
  }
  inline Short_t getPED(int ch_id){
      return PED[ch_id-1];
  }
};

typedef std::vector<Module_Config*> ModuleConfigFactory;
ModuleConfigFactory config_module;

void delete_config()
{
  int size=config_module.size();
  if(size>0){
    for(int i=0;i<size;i++){
      delete config_module[i];
    }
  }
  config_module.clear();
}

bool read_config(const char* filename)
{
    // Load adc configs
    ifstream fp;
    string tempstr;
    int tempint_2;
    int counter=0;
    Module_Config* tempconfig;
    
    // delete previous config if exists
    delete_config();

    //
    fp.open( filename );
    if( !fp )
    {
        return false;
    }

    //
    while( 1 )
    {
        fp >> tempstr;
        if( "CONFIG_BEGIN" == tempstr )
        {
            break;
        }
    }
    while("CONFIG_END" != tempstr)
    {
        fp >> tempstr;
        if(tempstr == "STATION")
        {
            counter++;
            tempconfig=new Module_Config();
            //first is station number
            fp >> tempstr;
            fp >> tempint_2;
            tempconfig->setStation(tempint_2);

            while(tempstr != "STATION_END")
            {
                fp >> tempstr;
                if(tempstr == "NAME"){
                    fp >> tempstr;
                    fp >> tempstr;
                    tempconfig->setName(tempstr);
                }
                else if(tempstr == "Channel"){
                    //discard header
                    while( 1 )
                    {
                        fp >> tempstr;
                        if( "Index" == tempstr )
                        {
                            break;
                        }
                    }
                    //read config data
                    tempint_2=99;
                    while(tempint_2 >=0)
                    {
                        fp >> tempint_2;

                        if ( 0 == tempint_2 )
                        {
                            UShort_t ctrl[3];
                            for (int i = 0; i < 3; ++i)
                            {
                                fp >> ( ctrl[ i ] );
                            }
                            UShort_t control=(ctrl[0]<<2) + (ctrl[1]<<1) + ctrl[2];
                            tempconfig->setCtrl(control);
                        }
                        else if( tempint_2 > 0 && tempint_2 <= 16 )
                        {
                            int tmp[4];
                            for (int i = 0; i < 4 ; ++i)
                            {
                                fp >> tmp[i];
                            }
                            tempconfig->setUT(tempint_2,tmp[0]);
                            tempconfig->setLT(tempint_2,tmp[1]);
                            tempconfig->setPED(tempint_2,tmp[2]);
                        }
                    }
                }
            }
            config_module.push_back(tempconfig);
        }
    }

    if(config_module.size() != counter) return false;

    fp.close();

    return true;
}


bool decoding_imp(const char* raw_filename,const char* root_filename)
{
  // terminator indicator bytes
    const unsigned short event_terminator=0xEEEE;
    const unsigned short buffer_terminator=0xFFFF;
    //
    FILE* fp_raw;
    TFile* file_out;
    //
    int card_num = config_module.size();
    TH1F* hist=new TH1F[card_num*16];
    int* datatmp = new int[card_num*16];
    UInt_t trigger_id;
    //
    char buffer1[4096*2];
    char buffer_header[4];
    unsigned short* pBuffer;
    unsigned short* pEvent;
    //
    int event_num,event_length,buffer_length;
    //
    file_out=new TFile(root_filename,"recreate");
    
    TTree* tree_out=new TTree("camac","CAMAC Raw ADC Data");
    tree_out->Branch("trigger_id",&trigger_id,"trigger_id/i");
    
    for (int i = 0; i < card_num; i++) {
      // init branches, each branch corresponds to one card and the branch name is the name of the card
      TString card_name = config_module[i]->getName();
      card_name.ReplaceAll(" ","_");
      tree_out->Branch(card_name.Data(), datatmp+i*16, Form("%s[16]/I",card_name.Data()));
      
      // create directory to save the histograms
      TDirectory* dir = file_out->mkdir(Form("histograms_%s",card_name.Data())); 
      
      // init histograms
      for (int ch_id=0; ch_id<16; ch_id++) {
        hist[i*16+ch_id].SetName(Form("h_%s_%d",card_name.Data(),ch_id+1));
        hist[i*16+ch_id].SetTitle(Form("%s Channel_%d in Station_%d",card_name.Data(),ch_id+1,config_module[i]->getStation()));
        hist[i*16+ch_id].SetBins(4098, -1.5 ,4096.5);
        hist[i*16+ch_id].SetDirectory(dir);
      }
    }

    //
    fp_raw=fopen(raw_filename,"rb");
    if(!fp_raw){
        printf("error! opening %s\n",raw_filename);
        delete [] datatmp;
        return false;
    }

    fread(buffer_header,sizeof(char),4,fp_raw);
    pBuffer=(unsigned short*)buffer_header;
    // first word of a buffer contains two infos:
    //  1) buffer type: bit_15=1-->watchdog buffer, bit_14=0-->data buffer, bit_14=1-->scaler buffer
    //  2) event number in this buffer: lower 12 bits
    event_num=pBuffer[0]&0xFFF;
    // second word of a buffer contains the length of the remaining buffer in the unit of word.
    buffer_length=pBuffer[1]&0xFFF;

    int i,j,k,ch_id;
    while(!feof(fp_raw) && !ferror(fp_raw)){
        fread(buffer1,sizeof(unsigned short),buffer_length,fp_raw);
        pBuffer=(unsigned short*)buffer1;
        if(pBuffer[buffer_length-1]!=buffer_terminator){
            printf("buffer error,%d,0x%x\n",buffer_length,pBuffer[buffer_length-1]);
            fclose(fp_raw);
            return false;
        }
        else{
            pEvent=pBuffer;
            for(i=0;i<event_num;i++){
                // init datatmp
                for(k=0;k<card_num*16;k++){
                    datatmp[k]=-1000;
                }
                //
                event_length=pEvent[0];
                if(pEvent[event_length] != event_terminator){
                    printf("event structure error\n");
                }
                else if(event_length != (card_num*16 + 3)){
                    printf("event length erorr\n");
                }
                else{
                    trigger_id=pEvent[1]+(pEvent[2]<<16);
                    for (k=0; k < card_num; k++) {
                      for (j=0; j<16; j++) {
                        ch_id=pEvent[3+k*16+j]>>12;
                        datatmp[k*16+ch_id]=pEvent[3+k*16+j]&0xFFF;
                        if(ch_id != j){
                          printf("channel sequence error\n");
                        }
                      }
                    }
                }
                //
                tree_out->Fill();
                for(k=0;k<card_num*16;k++){
                    hist[k].Fill(datatmp[k]);
                }
                if(trigger_id%5000==0){
                    printf("%d events decoded\n",trigger_id);
                }
                //
                pEvent=pEvent+event_length+1;
            }
        }
        //
        fread(buffer_header,sizeof(char),4,fp_raw);
        pBuffer=(unsigned short*)buffer_header;
        event_num=pBuffer[0]&0xFFF;
        buffer_length=pBuffer[1]&0xFFF;
    }
    printf("decoding ended successfully, total events: %d",trigger_id);
     
    fclose(fp_raw);
    
    //
    file_out->Write();
    delete file_out;
    delete []datatmp;
    
    return true;
}

bool decoding(const char* infile, const char* outfile, const char* config_file)
{
  if (!read_config(config_file) || !decoding_imp(infile, outfile)) {
    return false;
  }

  return true;
}

// chan_id start from 1
TH1F* draw_imp(const char* filename, const char* card_name, const int chan_id)
{
  TFile* infile=new TFile(filename);
  TDirectory* dir=infile->GetDirectory(Form("histograms_%s",card_name));
  TH1F* hist=(TH1F*)dir->Get(Form("h_%s_%d",card_name,chan_id));
  hist->SetDirectory(0);

  delete infile;

  return hist;
}

////////////////////////////////////////
// The following functions are intended for PMT batch test
////////////////////////////////////////
typedef std::vector<int> PMT_Config;
PMT_Config read_pmtconfig(const char* filename)
{
  FILE* fp=fopen(filename, "r");
  if (!fp) {
    printf("PMT Config file not exist: %s\n", filename);
    exit(1);
  }

  int steps, voltage;
  PMT_Config pmt_config;
  fscanf(fp, "Voltage Step: %d\n", &steps);
  for (int i=0; i < steps; i++) {
    fscanf(fp, "%dV\n", &voltage);
    pmt_config.push_back(voltage);
  }

  fclose(fp);

  return pmt_config;
}

bool RawDataConv(const char* parentDir, const char* adc_config_file)
{
    if(!(gSystem->OpenDirectory(parentDir))){
        printf("error: can't find directory %s\n",parentDir);
        printf("Please check if this is the correct directory!\n");
        return false;
    }
 
    Int_t datapoints;
    char raw_dir[300];
    char root_dir[300];
    char buffer1[300];
    char buffer2[300];
    char infile[256];
    char outfile[256];

    FILE* fp;
    sprintf(raw_dir,"%s/raw_data",parentDir);
    sprintf(root_dir,"%s/root_file",parentDir);
    if(!(gSystem->OpenDirectory(root_dir)))
        gSystem->MakeDirectory(root_dir);
    sprintf(buffer1,"%s/configuration.csv",raw_dir);
    sprintf(buffer2,"%s/configuration.csv",root_dir);
    gSystem->CopyFile(buffer1,buffer2);
    sprintf(buffer1,"%s/pmt.conf",raw_dir);
    sprintf(buffer2,"%s/pmt.conf",root_dir);
    gSystem->CopyFile(buffer1,buffer2);

    // read pmt.conf file to get all the testing voltage steps
    PMT_Config pmt_config = read_pmtconfig(Form("%s/pmt.conf",raw_dir));
    int HIGHVOLTAGE_STEP = pmt_config.size();
    int* VOLTAGE = new int[HIGHVOLTAGE_STEP];
    for (int i=0; i < HIGHVOLTAGE_STEP; i++) {
      VOLTAGE[i] = pmt_config[i];
    }
    for(int i=0;i<HIGHVOLTAGE_STEP;i++){
        //------------------PMT testing data converting-------------------------------
        sprintf(buffer1,"%s/%dV/LED.config",raw_dir,VOLTAGE[i]);
        if(!(fp=fopen(buffer1,"r"))){
            printf("error: can't open %s!\n",buffer1);
            return false;
        }
        fgets(buffer2,200,fp);
        fscanf(fp,"Total datapoints: %d\n",&datapoints);
        fclose(fp);

        sprintf(buffer2,"%s/%dV",root_dir,VOLTAGE[i]);
        gSystem->MakeDirectory(buffer2);
        sprintf(buffer2,"%s/%dV/LED.config",root_dir,VOLTAGE[i]);
        gSystem->CopyFile(buffer1,buffer2);

        sprintf(buffer1,"%s/%dV",raw_dir,VOLTAGE[i]);
        sprintf(buffer2,"%s/%dV",root_dir,VOLTAGE[i]);
        for(int j=0;j<datapoints;j++){
          sprintf(infile,"%s/%d.dat",buffer1,j+1);
          sprintf(outfile,"%s/%d.root",buffer2,j+1);
          decoding(infile,outfile,adc_config_file);
        }

    }

    //-------------------pedestal data converting----------------------------------
    sprintf(buffer2,"%s/pedestal",root_dir);
    gSystem->MakeDirectory(buffer2);
    sprintf(buffer1,"%s/pedestal",raw_dir);

    decoding(Form("%s/begin.dat",buffer1),Form("%s/begin.root",buffer2),adc_config_file);
    decoding(Form("%s/end.dat",buffer1),Form("%s/end.root",buffer2),adc_config_file);

    printf("All the raw data of %s has been converted.\n",parentDir);
    printf("The root files are saved at %s\n",root_dir);

    delete VOLTAGE;
    return true;
}

// read configuration.csv file for root file analysis
typedef struct 
{
  int  test_channel;
  char label[100];
  char card_name[100];
  int  card_channel;
} PMTInfo_t;
typedef std::vector<PMTInfo_t> PMTInfo;

PMTInfo read_analysis_config(const char* filename)
{
    FILE* fp=fopen(filename,"r");
    if(!fp){
        printf("error opening %s\n",filename);
        exit(1);
    }

    char buffer1[300];
    fgets(buffer1,200,fp);
    printf("%s",buffer1);
    fgets(buffer1,200,fp);
    printf("%s",buffer1);

    PMTInfo_t pmtinfo;
    PMTInfo   pmt_analysis_config;
    while (!feof(fp)) {
      fscanf(fp,"%d %s %s %d\n",&pmtinfo.test_channel, pmtinfo.label, pmtinfo.card_name, &pmtinfo.card_channel);
      printf("%d %s %s %d\n",pmtinfo.test_channel, pmtinfo.label, pmtinfo.card_name, pmtinfo.card_channel);
      pmt_analysis_config.push_back(pmtinfo);
    }
    
    fclose(fp);
    return pmt_analysis_config;
}

bool Fit_TestingData(const char* parentDir)
{
    char root_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);

    gStyle->SetOptFit(11);

    PMTInfo pmtinfo = read_analysis_config(Form("%s/configuration.csv",root_dir));
    PMT_Config test_config = read_pmtconfig(Form("%s/pmt.conf",root_dir));

   //---------------------------------Project corresponding hist--------------------------
    Int_t datapoints;
   Int_t* index;
   Float_t* width,*high_level;
   Float_t temp1,temp2,temp3,temp4;
   Char_t ledconfig[200],infile[200],outfilePDF[200],outfileDAT[200],outfileNoPed[200];
   FILE* fp_ledconfig;
   FILE* fp;
   FILE* fp_noped;

   int CHANNEL_NUM = pmtinfo.size();
   TH1F** hist = new TH1F*[CHANNEL_NUM];
   TSpectrum* s=new TSpectrum(1);
   TF1 *fitfunc,*fgaus;
   Int_t nfound,bin;
   Float_t *xpeaks;
   Float_t xp,yp,xmin,xmax,sigma;

   TCanvas* canvas=new TCanvas("canvas","Peaking of PMT",600,600);

   int HIGHVOLTAGE_STEP = test_config.size();
   int* VOLTAGE = new int[HIGHVOLTAGE_STEP];
   for (int i=0; i < HIGHVOLTAGE_STEP; i++) {
     VOLTAGE[i] = test_config[i];
   }
   for(int i=0;i<HIGHVOLTAGE_STEP;i++){
       //------------------------read LED.config-----------------------------------------------
       sprintf(ledconfig,"%s/%dV/LED.config",root_dir,VOLTAGE[i]);
       if(!(fp_ledconfig=fopen(ledconfig,"r"))){
           printf("error: can't open %s\n",ledconfig);
           return false;
       }
       fgets(buffer1,200,fp_ledconfig);
       printf("%s",buffer1);
       fscanf(fp_ledconfig,"Total datapoints: %d\n",&datapoints);
       printf("Total datapoints: %d\n",datapoints);
       fgets(buffer1,200,fp_ledconfig);
       printf("%s",buffer1);
       index = new int[datapoints];
       width = new float[datapoints];
       high_level = new float[datapoints];
       for(int j=0;j<datapoints;j++){
           fscanf(fp_ledconfig,"%d %E %E %E %E %E %E\n",index+j,&temp1,width+j,high_level+j,&temp2,&temp3,&temp4);
           printf("%d %f %5.2f %f %f %.12f %.12f\n",index[j],temp1,width[j]*1e6,high_level[j],temp2,temp3,temp4);
       }
       fclose(fp_ledconfig);
       
       //-----------------------------------------------------------------------------------------------
       for(int j=0;j<datapoints;j++){
           sprintf(infile,"%s/%dV/%d.root",root_dir,VOLTAGE[i],j+1);
           sprintf(outfilePDF,"%s/%dV/%d_result.pdf",root_dir,VOLTAGE[i],j+1);
           sprintf(outfileNoPed,"%s/%dV/%d_noped.dat",root_dir,VOLTAGE[i],j+1);

           for(int k=0;k<CHANNEL_NUM;k++){
             hist[k]= draw_imp(infile, pmtinfo[k].card_name, pmtinfo[k].card_channel);
             hist[k]->SetTitle(Form("%dV,%5.2fus,%3.2fV,PMT_%s, %s_%d,Testing_Channel %d",VOLTAGE[i],width[j]*1e6,high_level[j],pmtinfo[k].label,pmtinfo[k].card_name,pmtinfo[k].card_channel,pmtinfo[k].test_channel));
           }
           //------------------------------find peak and save/print the result--------------------
           sprintf(buffer1,"%s[",outfilePDF);
           canvas->Print(buffer1);

           fp_noped=fopen(outfileNoPed,"w");
           if(!fp_noped){
               printf("error: can't create %s\n",outfileNoPed);
               return false;
           }
           fprintf(fp_noped,"No Pedestal,fitting result of %d.dat at %5.2fus,%3.2fV:\n",j+1,width[j]*1e6,high_level[j]);
           char* tmp="Ch\tPMT\tMean\tSigma\n";
           fputs(tmp,fp_noped);

           for(int k=0;k<CHANNEL_NUM;k++){
               nfound = s->Search(hist[k],2,"goff",0.05);
               if(nfound != 1){
                   printf("In file %s/%dV/%d.dat, PMT_%s:\n",root_dir,VOLTAGE[i],j+1,pmtinfo[k].card_name);
                   printf("error: %d peak was found!\n",nfound);
                   return false;
               }
               xpeaks = s->GetPositionX();
               xp = xpeaks[0];
               bin = hist[k]->GetXaxis()->FindBin(xp);
               yp = hist[k]->GetBinContent(bin);
               xmin = xp-100;
               xmax = xp+100;
               hist[k]->Fit("gaus","q","",xmin,xmax);
               fitfunc = (TF1*)hist[k]->GetFunction("gaus");
               sigma = fitfunc->GetParameter(2);
               xmin = xp-5*sigma;
               xmax = xp+5*sigma;
               fgaus = new TF1("fgaus","gaus",xmin,xmax);
               fgaus->SetNpx(1000);
               hist[k]->Fit("fgaus","q");

               fprintf(fp_noped,"%d\t%s\t%f\t%f\n",pmtinfo[k].test_channel,pmtinfo[k].label,fgaus->GetParameter(1),fgaus->GetParameter(2));
               canvas->Print(outfilePDF);

               delete fgaus;
           }

           fclose(fp_noped);
           sprintf(buffer1,"%s]",outfilePDF);
           canvas->Print(buffer1);
           
          for(int k=0;k<CHANNEL_NUM;k++){
               delete hist[k];
           }
 
       }
       delete [] index;
       delete [] width;
       delete [] high_level;

   }

   delete [] hist;
   delete canvas;
   //-------------------------------------------------------------------------------------
   return true;
}

bool Fit_PedestalData(const Char_t *parentDir)
{
    char root_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    gStyle->SetOptFit(11);
   //--------------------------------read testing configuration------------------------
    PMTInfo pmtinfo = read_analysis_config(Form("%s/configuration.csv",root_dir));
    PMT_Config test_config = read_pmtconfig(Form("%s/pmt.conf",root_dir));
   //---------------------------------Project corresponding hist--------------------------
   Char_t prefix[2][20]={"begin","end"};
   Char_t infile[100],outfilePDF[100],outfileDAT[100];
   FILE* fp;

      int CHANNEL_NUM = pmtinfo.size();
   TH1F** hist = new TH1F*[CHANNEL_NUM];
TSpectrum* s=new TSpectrum(1);
   TF1 *fitfunc,*fgaus;
   Int_t nfound,bin;
   Float_t *xpeaks;
   Float_t xp,yp,xmin,xmax,sigma;

   TCanvas* canvas=new TCanvas("canvas","Pedestal",600,600);

   for(int i=0;i<2;i++){
       sprintf(infile,"%s/pedestal/%s.root",root_dir,prefix[i]);
       printf("%s\n",infile);
       sprintf(outfilePDF,"%s/pedestal/%s_result.pdf",root_dir,prefix[i]);
       sprintf(outfileDAT,"%s/pedestal/%s_result.dat",root_dir,prefix[i]);

       for(int j=0;j<CHANNEL_NUM;j++){
      hist[j]= draw_imp(infile, pmtinfo[j].card_name, pmtinfo[j].card_channel);
      hist[j]->SetTitle(Form("PMT_%s, %s_%d,Testing_Channel %d",pmtinfo[j].label,pmtinfo[j].card_name,pmtinfo[j].card_channel,pmtinfo[j].test_channel));
}
       sprintf(buffer1,"%s[",outfilePDF);
       canvas->Print(buffer1);

       fp=fopen(outfileDAT,"w");
       if(!fp){
           printf("error:creating %s\n",outfileDAT);
           return false;
       }
       fprintf(fp,"Fitting result of pedestal:\n");
       char* tmp="Ch\tPMT\tMean\tSigma\n";
       fputs(tmp,fp);

       for(int k=0;k<CHANNEL_NUM;k++){

           nfound = s->Search(hist[k],2,"goff",0.05);
           if(nfound != 1){
             printf("In file %s/pedestal/%s.root, PMT%s pedestal:\n",root_dir,prefix[i],pmtinfo[k].label);
               printf("error: %d peak was found!\n",nfound);
               return false;
           }
           xpeaks = s->GetPositionX();
           xp = xpeaks[0];
           bin = hist[k]->GetXaxis()->FindBin(xp);
           yp = hist[k]->GetBinContent(bin);
           xmin = xp-100;
           xmax = xp+100;
           hist[k]->Fit("gaus","q","",xmin,xmax);
           fitfunc = (TF1*)hist[k]->GetFunction("gaus");
           sigma = fitfunc->GetParameter(2);
           xmin = xp-3*sigma;
           xmax = xp+3*sigma;
           fgaus = new TF1("fgaus","gaus",xmin,xmax);
           fgaus->SetNpx(1000);
           hist[k]->Fit("fgaus","q");

           fprintf(fp,"%d\t%s\t%f\t%f\n",pmtinfo[k].test_channel,pmtinfo[k].label,fgaus->GetParameter(1),fgaus->GetParameter(2));
           canvas->Print(outfilePDF);

           delete fgaus;
       }
       fclose(fp);
       sprintf(buffer1,"%s]",outfilePDF);
       canvas->Print(buffer1);

       for(int k=0;k<CHANNEL_NUM;k++){
           delete hist[k];
       }


   }
    delete canvas;

   return true;
}
