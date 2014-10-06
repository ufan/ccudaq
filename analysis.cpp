#include "stdio.h"
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
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
#include "TSQLServer.h"
#include "TMath.h"
#include <vector>
#include <map>


using namespace std;
/****************Global Variables*********************************************/
//---------------PMT testing system configuration info-------------------------------------------------------------
const Int_t CHANNEL_NUM=22;
const Int_t RefPMT_NO=2;
const Int_t RefPMT_CH[RefPMT_NO]={1,2};//0-->Channel_1  ,1-->Channel_2
const Int_t TestPMT_NO=20;
const Int_t TestPMT_CH[TestPMT_NO]={3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
const Int_t HIGHVOLTAGE_STEP=8;
const Int_t VOLTAGE[HIGHVOLTAGE_STEP]={1200,1300,1400,1500,1600,1700,1800,1900};
const Int_t MAXIMUM_DATAPOINTS=25;
const Int_t GAIN_FILENO=5;
const Int_t GAIN_FILEINDEX[HIGHVOLTAGE_STEP][GAIN_FILENO]={{1,2,3,4,5},{1,2,3,4,5},{17,18,19,20,21}
                           ,{17,18,19,20,21},{17,18,19,20,21},{17,18,19,20,21}
                           ,{17,18,19,20,21},{1,2,3,4,5}};
const Int_t DY58_FILENO[HIGHVOLTAGE_STEP]={15,15,16,16,16,16,16,11};
const Int_t TOTAL_FILENO[HIGHVOLTAGE_STEP]={11,11,11,11,11,11,11,11};
const Int_t GROUP1_FILENO[HIGHVOLTAGE_STEP]={10,10,11,11,11,11,11,11};
const Int_t GROUP1_FILEINDEX[HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS]={{1,2,3,4,5,6,7,8,9,10},{1,2,3,4,5,6,7,8,9,10},{1,2,3,4,5,6,7,8,9,10,11},{1,2,3,4,5,6,7,8,9,10,11}
                                                                   ,{1,2,3,4,5,6,7,8,9,10,11},{1,2,3,4,5,6,7,8,9,10,11},{1,2,3,4,5,6,7,8,9,10,11},{1,2,3,4,5,6,7,8,9,10,11}};
const Int_t GROUP2_FILENO[HIGHVOLTAGE_STEP]={5,5,5,5,5,5,5,5};
const Int_t GROUP2_FILEINDEX[HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS]={{11,12,13,14,15},{11,12,13,14,15},{12,13,14,15,16},{12,13,14,15,16}
                                                                   ,{12,13,14,15,16},{12,13,14,15,16},{12,13,14,15,16},{12,13,14,15,16}};
const Int_t GROUP3_FILENO[HIGHVOLTAGE_STEP]={5,5,5,5,5,5,5,5};
const Int_t GROUP3_FILEINDEX[HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS]={{1,2,3,4,5},{1,2,3,4,5},{17,18,19,20,21},{17,18,19,20,21},{17,18,19,20,21}
                                                                   ,{17,18,19,20,21},{17,18,19,20,21},{17,18,19,20,21}};
const Float_t CHANNEL_DIFF[CHANNEL_NUM]={1.0,1.2775765809,1.121589502,0.9579736337,1.0262718486,1.4080747544,1.9571727843,1.7562655577,1.3086306669,0.913844951,1.072783158,
                                         0.9156117824,1.1635673492,1.1189168228,1.0662695461,1.124735192,1.1650342171,1.0904545829,1.0628654897,1.0426446949,1.0697705836};
//-----------------------------------------------------------------------------------------------------------------

//---------------PMT testing channel configuration info-------------------------------------------------------------
Int_t TEST_CH[CHANNEL_NUM];
Int_t PMT_INDEX[CHANNEL_NUM];//this array is kept for later extendence.PMT_INDEX is the unique number associated with each pmt testing file.
Char_t PMT_LABEL[CHANNEL_NUM][50];
Char_t PMT_POSITION[CHANNEL_NUM][50];//the position of pmt in the box
Int_t FEE_DY5[CHANNEL_NUM],FEE_DY8[CHANNEL_NUM];
Int_t CONFIG_FLAG=0;//if the configuration.txt file has been read CONFIG_FLAG equals to 1,otherwise 0;
//------------------------------------------------------------------------------------------------------------------

//---------------Global Testing-Data buffer---------------------------------------------------------------------------------------
Float_t Data_Dy5[CHANNEL_NUM][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Float_t Data_Dy8[CHANNEL_NUM][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Float_t Sigma_Dy5[CHANNEL_NUM][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Float_t Sigma_Dy8[CHANNEL_NUM][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];

Float_t Data_Gain[CHANNEL_NUM][GAIN_FILENO][HIGHVOLTAGE_STEP];
Float_t Sigma_Data_Gain[CHANNEL_NUM][GAIN_FILENO][HIGHVOLTAGE_STEP];
Float_t Calibrated_Data_Gain_Step1[CHANNEL_NUM][GAIN_FILENO][HIGHVOLTAGE_STEP];
Float_t Calibrated_Data_Gain_Step2[CHANNEL_NUM][GAIN_FILENO][HIGHVOLTAGE_STEP];
Int_t DATAUPDATE_FLAG=0;//indicate whether there is valid data in buffer
Int_t TestDirectory_id=0;//indicate which test's data is in buffer

//Float_t Ref_Dy5[RefPMT_NO][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Float_t Ref_Dy8[RefPMT_NO][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Float_t Calib_Ratio[RefPMT_NO][HIGHVOLTAGE_STEP][MAXIMUM_DATAPOINTS];
Int_t REFDATAUPDATE_FLAG=0;
Int_t RefDirectory_id=0;
//-----------------All kinds of flags-----------------------------------------------------------------------------------------------------

enum RefPMT {AA2236=1,AA2233=2};
//RefPMT LED_CalibPMT=AA2236;
//RefPMT Channel_CalibPMT=AA2104;

//------------------MySQL info-------------------------------------------------------
Char_t dbname[100]="mysql://192.168.1.110/PMT_V2";
//Char_t dbname[100]="mysql://localhost/PMT_V2";
Char_t username[20]="root";
Char_t password[20]="111111";
/****************Global Functions*********************************************/

int decoding(const char* inDir,const char* infile,const char* outDir,const char* outfile)
{
    FILE* fp_raw;
    TFile* file_out;
    TTree* tree_out;
    TH1F* hist[32];
    int datatmp[32],trigger_id;
    TString raw_filename,out_filename;
    char buffer1[4096*2];
    char buffer_header[4];
    unsigned short* pBuffer;
    unsigned short* pEvent;

    int event_num,event_length,buffer_length;
    unsigned short event_terminator=0xEEEE;
    unsigned short buffer_terminator=0xFFFF;
    //
    out_filename=Form("%s/%s",outDir,outfile);
    file_out=new TFile(out_filename.Data(),"recreate");
    tree_out=new TTree("PMT_CAMAC","PMT_CAMAC");
    tree_out->Branch("event",datatmp,"hh[32]/I");
    tree_out->Branch("trigger_id",&trigger_id,"trigger_id/I");

    TString hname,htitle;
    for(int i=0;i<32;i++){
        hname=Form("h%d",i+1);
        htitle=Form("h%d",i+1);
        hist[i]=new TH1F(hname,htitle,4098,-1.5,4096.5);
    }
    //
    raw_filename=Form("%s/%s",inDir,infile);
    fp_raw=fopen(raw_filename.Data(),"rb");
    if(!fp_raw){
        printf("error! opening %s\n",raw_filename.Data());
        return -1;
    }

    fread(buffer_header,sizeof(char),4,fp_raw);
    pBuffer=(unsigned short*)buffer_header;
    event_num=pBuffer[0]&0xFFF;
    buffer_length=pBuffer[1]&0xFFF;

    int i,j,k,ch_id;
    while(!feof(fp_raw) && !ferror(fp_raw)){
        fread(buffer1,sizeof(unsigned short),buffer_length,fp_raw);
        pBuffer=(unsigned short*)buffer1;
        if(pBuffer[buffer_length-1]!=buffer_terminator){
            printf("buffer error,%d,0x%x\n",buffer_length,pBuffer[buffer_length-1]);
            return -1;
        }
        else{
            pEvent=pBuffer;
            for(i=0;i<event_num;i++){
                //
                for(k=0;k<32;k++){
                    datatmp[k]=-1;
                }
                //
                event_length=pEvent[0];
                if(pEvent[event_length] != event_terminator){
                    printf("event error\n");
                }
                else if(event_length != 35){
                    printf("event length erorr\n");
                }
                else{
                    trigger_id=pEvent[1]+(pEvent[2]<<16);
                    for(j=0;j<16;j++){
                        ch_id=pEvent[3+j]>>12;
                        datatmp[ch_id]=pEvent[3+j]&0xFFF;
                        if(ch_id != j){
                            printf("channel error\n");
                        }
                    }
                    for(j=0;j<16;j++){
                        ch_id=pEvent[19+j]>>12;
                        datatmp[ch_id+16]=pEvent[19+j]&0xFFF;
                        if(ch_id != j){
                            printf("channel error\n");
                        }
                    }
                }
                //
                tree_out->Fill();
                for(k=0;k<32;k++){
                    hist[k]->Fill(datatmp[k]);
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

    //
    fclose(fp_raw);
    //
    file_out->Write();
    file_out->Close();

    delete file_out;
    /*
    delete tree_out;
    printf("here4");
    for(i=0;i<32;i++){
        delete hist[i];
    }
    printf("here5");
    */
    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//func: RawDataConv
//      converting raw testing data into rootfile,including pmt testing data and pedestal data.
//      A new directory is created "parentDir/root_file",and the directory structure of raw_data is
//      copied.
//input:
//      parentDir,the parent directory where raw testing data is stored
//output:
//      return 0 if no error occured,otherwise -1
///////////////////////////////////////////////////////////////////////////////////////////////////
int RawDataConv(const char* parentDir)
{
    if(!(gSystem->OpenDirectory(parentDir))){
        printf("error: can't find directory %s\n",parentDir);
        printf("Please check if this is the correct directory!\n");
        return -1;
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

    for(int i=0;i<HIGHVOLTAGE_STEP;i++){
        //------------------PMT testing data converting-------------------------------
        sprintf(buffer1,"%s/%dV/LED.config",raw_dir,VOLTAGE[i]);
        if(!(fp=fopen(buffer1,"r"))){
            printf("error: can't open %s!\n",buffer1);
            return -1;
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
            sprintf(infile,"%d.dat",j+1);
            sprintf(outfile,"%d.root",j+1);
            decoding(buffer1,infile,buffer2,outfile);
        }

    }

    //-------------------pedestal data converting----------------------------------
    sprintf(buffer2,"%s/pedestal",root_dir);
    gSystem->MakeDirectory(buffer2);
    sprintf(buffer1,"%s/pedestal",raw_dir);

    decoding(buffer1,"begin.dat",buffer2,"begin.root");
    decoding(buffer1,"end.dat",buffer2,"end.root");

    printf("All the raw data of %s has been converted.\n",parentDir);
    printf("The root files are saved at %s\n",root_dir);

    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
//func: ReadConfigFile
//      get the testing confiugration of testing data in parentDir
//input:
//      parentDir is the directory where testing data is stored
//output:
//      return 0 if no error occured, otherwise -1
/////////////////////////////////////////////////////////////////////////////////////////
int ReadConfigFile(const char* parentDir)
{
    Char_t buffer1[300];
    Char_t root_dir[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    //gSystem->ChangeDirectory(root_dir);//change to the root_file directory,all the analysis inputs are under this directory.
    sprintf(buffer1,"%s/configuration.csv",root_dir);
    FILE* fp=fopen(buffer1,"r");
    if(!fp){
        printf("error opening %s\n",buffer1);
        return -1;
    }

    fgets(buffer1,200,fp);
    printf("%s",buffer1);
    fgets(buffer1,200,fp);
    printf("%s",buffer1);
    for(int i=0;i < CHANNEL_NUM;i++){
        fscanf(fp,"%d %d %d\n",TEST_CH+i,PMT_INDEX+i,FEE_DY8+i);
        printf("%d %d %d\n",TEST_CH[i],PMT_INDEX[i],FEE_DY8[i]);
    }
    CONFIG_FLAG=1;

    fclose(fp);

    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//func: Fit_PedestalData
//      get the pedestal fitting result
//input:
//      parentDir is the parent directory of the testing data
//output:
//      return 0 if no error occured,otherwise -1
//////////////////////////////////////////////////////////////////////////////////////////////////////
int Fit_PedestalData(const Char_t *parentDir)
{
    char root_dir[300];
    char buffer1[300];
//--------------------Check if root_file dir has been created AND create convert raw data to root file-------------------------
    sprintf(root_dir,"%s/root_file",parentDir);
    //RawDataConv(parentDir);
//--------------------Fitting of root file-------------------------------------------------------------------------------------
    gStyle->SetOptFit(11);
   //--------------------------------read testing configuration------------------------
   ReadConfigFile(parentDir);
   //---------------------------------Project corresponding hist--------------------------
   Char_t prefix[2][20]={"begin","end"};
   Char_t infile[100],outfilePDF[100],outfileDAT[100];
   FILE* fp;

   TString htitlePedestalDy8,hnamePedestalDy8,branchPedestalDy8;
   TH1F* histPedestalDy8[CHANNEL_NUM];
   TFile* file;
   TTree* tree;
   TSpectrum* s=new TSpectrum(1);
   TF1 *fitfuncDy8,*fgausDy8;
   Int_t nfound,bin;
   Float_t *xpeaks;
   Float_t xp,yp,xmin,xmax,sigma;

   TCanvas* canvas=new TCanvas("canvas","Pedestal",600,600);

   for(int i=0;i<2;i++){
       sprintf(infile,"%s/pedestal/%s.root",root_dir,prefix[i]);
       printf("%s\n",infile);
       sprintf(outfilePDF,"%s/pedestal/%s_result.pdf",root_dir,prefix[i]);
       sprintf(outfileDAT,"%s/pedestal/%s_result.dat",root_dir,prefix[i]);

       file = new TFile(infile);
       tree = (TTree*)file->Get("PMT_CAMAC");
       tree->Print();
       for(int j=0;j<CHANNEL_NUM;j++){
           hnamePedestalDy8.Form("PMT%d_Pedestal",PMT_INDEX[j]);
           htitlePedestalDy8.Form("Measurement_%s Pedestal: PMT%d,CAMAC_CH %d,Test_CH %d",prefix[i],PMT_INDEX[j],FEE_DY8[j],TEST_CH[j]);
           branchPedestalDy8.Form("hh[%d]",FEE_DY8[j]-1);
           histPedestalDy8[j]=new TH1F(hnamePedestalDy8.Data(),htitlePedestalDy8.Data(),2000,-1000,3000);
           tree->Project(hnamePedestalDy8.Data(),branchPedestalDy8.Data());
       }
       sprintf(buffer1,"%s[",outfilePDF);
       canvas->Print(buffer1);

       fp=fopen(outfileDAT,"w");
       if(!fp){
           printf("error:creating %s\n",outfileDAT);
           return -1;
       }
       fprintf(fp,"Fitting result of pedestal:\n");
       char* tmp="Ch\tPMT\tMean\tSigma\n";
       fputs(tmp,fp);

       for(int k=0;k<CHANNEL_NUM;k++){

           nfound = s->Search(histPedestalDy8[k],2,"goff",0.05);
           if(nfound != 1){
               printf("In file %s/pedestal/%s.root, PMT%s pedestal:\n",root_dir,prefix[i],PMT_INDEX[k]);
               printf("error: %d peak was found!\n",nfound);
               return -1;
           }
           xpeaks = s->GetPositionX();
           xp = xpeaks[0];
           bin = histPedestalDy8[k]->GetXaxis()->FindBin(xp);
           yp = histPedestalDy8[k]->GetBinContent(bin);
           xmin = xp-100;
           xmax = xp+100;
           histPedestalDy8[k]->Fit("gaus","q","",xmin,xmax);
           fitfuncDy8 = (TF1*)histPedestalDy8[k]->GetFunction("gaus");
           sigma = fitfuncDy8->GetParameter(2);
           xmin = xp-3*sigma;
           xmax = xp+3*sigma;
           fgausDy8 = new TF1("fgausDy8","gaus",xmin,xmax);
           fgausDy8->SetNpx(1000);
           histPedestalDy8[k]->Fit("fgausDy8","q");
           //histPedestalDy8[k]->Fit("gaus","q","",100,500);
           //fitfuncDy8 = (TF1*)histPedestalDy8[k]->GetFunction("gaus");

           fprintf(fp,"%d\t%d\t%f\t%f\n",TEST_CH[k],PMT_INDEX[k],fgausDy8->GetParameter(1),fgausDy8->GetParameter(2));
           canvas->Print(outfilePDF);

           //delete fitfuncDy5;
           //delete fitfuncDy8;//for some reason,you can't delete fitfuncDy5/fitfuncDy8.Error will occur.
           delete fgausDy8;
       }
       fclose(fp);
       sprintf(buffer1,"%s]",outfilePDF);
       canvas->Print(buffer1);

       for(int k=0;k<CHANNEL_NUM;k++){
           delete histPedestalDy8[k];
       }

       file->Close();

   }
    delete canvas;

   return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//func: Fit_TestingData
//      fit the testing data and print out the result.the saved result has substracted pedestal value.
//input:
//      parentDir is the directory where the whole testing data files is saved.
//      when flag=0,the corresponding testing pedestal file will be used(begin.root/end.root);
//      when flag=1,the standard pedestal will be used;the default flag is 0.
//      pedfilename is the filename of corresponding pedestal file,meaningful when flag=0,otherwise it
//      is ignored.
//output:
//      return 0 is no error occured,otherwise -1
////////////////////////////////////////////////////////////////////////////////////////////////////////
int Fit_TestingData(const char* parentDir)
{
    char root_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);

//--------------------Check if root_file dir has been created AND create convert raw data to root file-------------------------
    sprintf(root_dir,"%s/root_file",parentDir);
//--------------------Fitting of root file-------------------------------------------------------------------------------------
    gStyle->SetOptFit(11);
   //--------------------------------read testing configuration------------------------
   ReadConfigFile(parentDir);
   //---------------------------------Project corresponding hist--------------------------
   Int_t datapoints,index[MAXIMUM_DATAPOINTS];
   Float_t width[MAXIMUM_DATAPOINTS],high_level[MAXIMUM_DATAPOINTS],temp1,temp2,temp3,temp4;
   Char_t ledconfig[200],infile[200],outfilePDF[200],outfileDAT[200],outfileNoPed[200];
   FILE* fp_ledconfig;
   FILE* fp;
   FILE* fp_noped;

   TString htitleDy8,hnameDy8,branchDy8;
   TH1F* histDy8[CHANNEL_NUM];
   TFile* file;
   TTree* tree;
   TSpectrum* s=new TSpectrum(1);
   TF1 *fitfuncDy8,*fgausDy8;
   Int_t nfound,bin;
   Float_t *xpeaks;
   Float_t xp,yp,xmin,xmax,sigma;

   TCanvas* canvas=new TCanvas("canvas","Peaking of PMT",600,600);

   for(int i=0;i<HIGHVOLTAGE_STEP;i++){
       //------------------------read LED.config------------------------------------------------------------
       sprintf(ledconfig,"%s/%dV/LED.config",root_dir,VOLTAGE[i]);
       if(!(fp_ledconfig=fopen(ledconfig,"r"))){
           printf("error: can't open %s\n",ledconfig);
           return -1;
       }
       fgets(buffer1,200,fp_ledconfig);
       printf("%s",buffer1);
       fscanf(fp_ledconfig,"Total datapoints: %d\n",&datapoints);
       printf("Total datapoints: %d\n",datapoints);
       fgets(buffer1,200,fp_ledconfig);
       printf("%s",buffer1);
       for(int j=0;j<datapoints;j++){
           fscanf(fp_ledconfig,"%d %E %E %E %E %E %E\n",index+j,&temp1,width+j,high_level+j,&temp2,&temp3,&temp4);
           printf("%d %f %5.2f %f %f %.12f %.12f\n",index[j],temp1,width[j]*1e6,high_level[j],temp2,temp3,temp4);
       }
       fclose(fp_ledconfig);
       //------------------------------------------------------------------------------------------------------------
       for(int j=0;j<datapoints;j++){
           sprintf(infile,"%s/%dV/%d.root",root_dir,VOLTAGE[i],j+1);
           sprintf(outfilePDF,"%s/%dV/%d_result.pdf",root_dir,VOLTAGE[i],j+1);
           sprintf(outfileNoPed,"%s/%dV/%d_noped.dat",root_dir,VOLTAGE[i],j+1);

           file=new TFile(infile);
           tree=(TTree*)file->Get("PMT_CAMAC");
           //tree->Print();

           for(int k=0;k<CHANNEL_NUM;k++){

               hnameDy8.Form("PMT_%d",PMT_INDEX[k]);
               htitleDy8.Form("%5.2fus,%3.2fV,PMT_%d,CAMAC_Channel %d,Testing_Channel %d",width[j]*1e6,high_level[j],PMT_INDEX[k],FEE_DY8[k],TEST_CH[k]);
               branchDy8.Form("hh[%d]",FEE_DY8[k]-1);
               histDy8[k]=new TH1F(hnameDy8.Data(),htitleDy8.Data(),4098,-1.5,4096.5);
               tree->Project(hnameDy8.Data(),branchDy8.Data());
           }
           //----------------------------------------------------------------------------------------------------
           //------------------------------find peak and save/print the result-------------------------------------------------------------
           sprintf(buffer1,"%s[",outfilePDF);
           canvas->Print(buffer1);

           fp_noped=fopen(outfileNoPed,"w");
           if(!fp_noped){
               printf("error: can't create %s\n",outfileNoPed);
               return -1;
           }
           fprintf(fp_noped,"No Pedestal,fitting result of %d.dat at %5.2fus,%3.2fV:\n",j+1,width[j]*1e6,high_level[j]);
           char* tmp="Ch\tPMT\tMean\tSigma\n";
           fputs(tmp,fp_noped);

           for(int k=0;k<CHANNEL_NUM;k++){

               nfound = s->Search(histDy8[k],2,"goff",0.05);
               if(nfound != 1){
                   printf("In file %s/%dV/%d.dat, PMT_%d:\n",root_dir,VOLTAGE[i],j+1,PMT_INDEX[k]);
                   printf("error: %d peak was found!\n",nfound);
                   return -1;
               }
               xpeaks = s->GetPositionX();
               xp = xpeaks[0];
               bin = histDy8[k]->GetXaxis()->FindBin(xp);
               yp = histDy8[k]->GetBinContent(bin);
               xmin = xp-100;
               xmax = xp+100;
               histDy8[k]->Fit("gaus","q","",xmin,xmax);
               fitfuncDy8 = (TF1*)histDy8[k]->GetFunction("gaus");
               sigma = fitfuncDy8->GetParameter(2);
               xmin = xp-5*sigma;
               xmax = xp+5*sigma;
               fgausDy8 = new TF1("fgausDy8","gaus",xmin,xmax);
               fgausDy8->SetNpx(1000);
               histDy8[k]->Fit("fgausDy8","q");

               fprintf(fp_noped,"%d\t%d\t%f\t%f\n",TEST_CH[k],PMT_INDEX[k],fgausDy8->GetParameter(1),fgausDy8->GetParameter(2));
               canvas->Print(outfilePDF);

               delete fgausDy8;
           }

           fclose(fp_noped);
           sprintf(buffer1,"%s]",outfilePDF);
           canvas->Print(buffer1);
           for(int k=0;k<CHANNEL_NUM;k++){
               delete histDy8[k];
           }
           file->Close();
       }
   }

   delete canvas;
   //-------------------------------------------------------------------------------------
   return 0;
}

//-----------------------------------------------------------------------------------
int ReadIntoDataBuffer(const Char_t* parDir,Int_t testdir_id){

    Char_t test_dir[300];
    Char_t root_dir[300];
    Char_t filename[300];
    Char_t buffer[200];
    TestDirectory_id=testdir_id;

    sprintf(test_dir,"%s",gSystem->ExpandPathName(parDir));
    sprintf(root_dir,"%s/root_file",test_dir);
    //--------------------
    ReadConfigFile(test_dir);
    //----------------------
    FILE* fp_noped;
    Int_t tmp_ch;
    Char_t tmp_label[20];
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        for(int Amplitude_id=0;Amplitude_id<TOTAL_FILENO[HV_id];Amplitude_id++){
            sprintf(filename,"%s/%dV/%d_noped.dat",root_dir,VOLTAGE[HV_id],Amplitude_id+1);
            fp_noped=fopen(filename,"r");
            if(!fp_noped){
                printf("error opening %s!\n",filename);
            }
            fgets(buffer,200,fp_noped);
            //printf("%s",buffer);
            fgets(buffer,200,fp_noped);
            //printf("%s",buffer);
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                fscanf(fp_noped,"%d %s %f %f %f %f\n",&tmp_ch,tmp_label,&(Data_Dy5[PMT_id][HV_id][Amplitude_id]),&(Sigma_Dy5[PMT_id][HV_id][Amplitude_id])
                       ,&(Data_Dy8[PMT_id][HV_id][Amplitude_id]),&(Sigma_Dy8[PMT_id][HV_id][Amplitude_id]));
                //printf("%d %s %f %f %f %f\n",tmp_ch,tmp_label,(Data_Dy5[PMT_id][HV_id][Amplitude_id]),(Sigma_Dy5[PMT_id][HV_id][Amplitude_id])
                //       ,(Data_Dy8[PMT_id][HV_id][Amplitude_id]),(Sigma_Dy8[PMT_id][HV_id][Amplitude_id]));
            }
            fclose(fp_noped);
        }
    }

    for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
        for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
            for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
                Data_Gain[PMT_id][Amplitude_id][HV_id]=Data_Dy8[PMT_id][HV_id][GAIN_FILEINDEX[HV_id][Amplitude_id]-1];
                Sigma_Data_Gain[PMT_id][Amplitude_id][HV_id]=Sigma_Dy8[PMT_id][HV_id][GAIN_FILEINDEX[HV_id][Amplitude_id]-1];
            }
        }
    }

    DATAUPDATE_FLAG=1;
    printf("Testing data have been read into Global Buffer\n");

    return 0;
}

int ReadRefIntoDataBuffer(const Char_t* parDir,Int_t refdir_id)
{
    Char_t test_dir[300];
    Char_t root_dir[300];
    Char_t filename[300];
    Char_t buffer[200];

    sprintf(test_dir,"%s",gSystem->ExpandPathName(parDir));
    sprintf(root_dir,"%s/root_file",test_dir);

    ReadConfigFile(test_dir);
    RefDirectory_id=refdir_id;

    FILE* fp_noped;
    Int_t tmp_ch;
    Char_t tmp_label[20];
    Float_t tmp_dy5_mean,tmp_dy5_sigma,tmp_dy8_mean,tmp_dy8_sigma;
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        for(int Amplitude_id=0;Amplitude_id<TOTAL_FILENO[HV_id];Amplitude_id++){
            sprintf(filename,"%s/%dV/%d_noped.dat",root_dir,VOLTAGE[HV_id],Amplitude_id+1);
            fp_noped=fopen(filename,"r");
            if(!fp_noped){
                printf("error opening %s!\n",filename);
            }
            fgets(buffer,200,fp_noped);
            //printf("%s",buffer);
            fgets(buffer,200,fp_noped);
            //printf("%s",buffer);
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                fscanf(fp_noped,"%d %s %f %f %f %f\n",&tmp_ch,tmp_label,&tmp_dy5_mean,&tmp_dy5_sigma
                       ,&tmp_dy8_mean,&tmp_dy8_sigma);
                for(int i=0;i<RefPMT_NO;i++){
                    if(RefPMT_CH[i] == TEST_CH[PMT_id]){
                        //Ref_Dy5[i][HV_id][Amplitude_id]=tmp_dy5_mean;
                        Ref_Dy8[i][HV_id][Amplitude_id]=tmp_dy8_mean;
                    }
                }
            }
            fclose(fp_noped);
        }
    }

    REFDATAUPDATE_FLAG=1;
    return 0;
}

//-----------------Gain Method1-----------------------------------
int Get_Gain_Method1_All(const char* parentDir,const Int_t testdir_id)
{
    ReadConfigFile(parentDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(parentDir,testdir_id);
    }
    //---------------------
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //-----------------------
    FILE* fp;
    char dir[300];

    TString gtitle;
    TGraph* ggain[CHANNEL_NUM];
    Float_t ratio;
    Float_t tmp_mean[MAXIMUM_DATAPOINTS],tmp_variance[MAXIMUM_DATAPOINTS];
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","Gain Method_1 (ALL)",500,600);
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_gain_method1_all.dat",dir,VOLTAGE[HV_id]);
        sprintf(outPDF,"%s/%dV_gain_method1_all.pdf",dir,VOLTAGE[HV_id]);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV gain result(method1,all):\n",VOLTAGE[HV_id]);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
            if(PMT_INDEX[PMT_id]==0){
                fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
            }
            else{
                for(int Amplitude_id=0;Amplitude_id<TOTAL_FILENO[HV_id];Amplitude_id++){
                    tmp_mean[Amplitude_id]=Data_Dy8[PMT_id][HV_id][Amplitude_id];
                    tmp_variance[Amplitude_id]=TMath::Power(Sigma_Dy8[PMT_id][HV_id][Amplitude_id],2);
                }
                gtitle.Form("Gain: Channel_%d, %dV, %s (method1,all)",PMT_id+1,VOLTAGE[HV_id],PMT_LABEL[PMT_id]);
                ggain[PMT_id]=new TGraph(TOTAL_FILENO[HV_id],tmp_mean,tmp_variance);
                ggain[PMT_id]->SetTitle(gtitle.Data());
                ggain[PMT_id]->Draw("A*");
                ggain[PMT_id]->Fit("pol1","","",0,12000);
                TF1* fitfuc=(TF1*)ggain[PMT_id]->GetFunction("pol1");
                ratio=fitfuc->GetParameter(1);
                fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],ratio);
                canvas->Print(outPDF);
                delete ggain[PMT_id];
            }
        }
        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);
    }

    return 0;
}

int Get_Gain_Method1_Group1(const char* parentDir,const Int_t testdir_id)
{
    ReadConfigFile(parentDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(parentDir,testdir_id);
    }
    //---------------------
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //---------------------------
    FILE* fp;
    char dir[300];

    TString gtitle;
    TGraph* ggain[CHANNEL_NUM];
    Float_t ratio;
    Float_t tmp_mean[MAXIMUM_DATAPOINTS],tmp_variance[MAXIMUM_DATAPOINTS];
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","Gain Method_1(Group_1)",500,600);
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_gain_method1_group1.dat",dir,VOLTAGE[HV_id]);
        sprintf(outPDF,"%s/%dV_gain_method1_group1.pdf",dir,VOLTAGE[HV_id]);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV gain result(method1,group1):\n",VOLTAGE[HV_id]);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
            if(PMT_INDEX[PMT_id]==0){
                fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
            }
            else{
                for(int Amplitude_id=0;Amplitude_id<GROUP1_FILENO[HV_id];Amplitude_id++){
                    tmp_mean[Amplitude_id]=Data_Dy8[PMT_id][HV_id][GROUP1_FILEINDEX[HV_id][Amplitude_id]-1];
                    tmp_variance[Amplitude_id]=TMath::Power(Sigma_Dy8[PMT_id][HV_id][GROUP1_FILEINDEX[HV_id][Amplitude_id]-1],2);
                }
                gtitle.Form("Gain: Channel_%d, %dV, %s (method1, group1)",PMT_id+1,VOLTAGE[HV_id],PMT_LABEL[PMT_id]);
                ggain[PMT_id]=new TGraph(GROUP1_FILENO[HV_id],tmp_mean,tmp_variance);
                ggain[PMT_id]->SetTitle(gtitle.Data());
                ggain[PMT_id]->Draw("A*");
                ggain[PMT_id]->Fit("pol1","","",0,12000);
                TF1* fitfuc=(TF1*)ggain[PMT_id]->GetFunction("pol1");
                ratio=fitfuc->GetParameter(1);
                fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],ratio);
                canvas->Print(outPDF);
                delete ggain[PMT_id];
            }
        }
        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);
    }

    return 0;
}

int Get_Gain_Method1_Group2(const char* parentDir,const Int_t testdir_id)
{
    ReadConfigFile(parentDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(parentDir,testdir_id);
    }
    //---------------------
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //---------------------------
    FILE* fp;
    char dir[300];

    TString gtitle;
    TGraph* ggain[CHANNEL_NUM];
    Float_t ratio;
    Float_t tmp_mean[MAXIMUM_DATAPOINTS],tmp_variance[MAXIMUM_DATAPOINTS];
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","Gain Method_1(Group_2)",500,600);
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_gain_method1_group2.dat",dir,VOLTAGE[HV_id]);
        sprintf(outPDF,"%s/%dV_gain_method1_group2.pdf",dir,VOLTAGE[HV_id]);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV gain result(method1,group2):\n",VOLTAGE[HV_id]);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
            if(PMT_INDEX[PMT_id]==0){
                fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
            }
            else{
                for(int Amplitude_id=0;Amplitude_id<GROUP2_FILENO[HV_id];Amplitude_id++){
                    tmp_mean[Amplitude_id]=Data_Dy8[PMT_id][HV_id][GROUP2_FILEINDEX[HV_id][Amplitude_id]-1];
                    tmp_variance[Amplitude_id]=TMath::Power(Sigma_Dy8[PMT_id][HV_id][GROUP2_FILEINDEX[HV_id][Amplitude_id]-1],2);
                }
                gtitle.Form("Gain: Channel_%d, %dV, %s (method1, group2)",PMT_id+1,VOLTAGE[HV_id],PMT_LABEL[PMT_id]);
                ggain[PMT_id]=new TGraph(GROUP2_FILENO[HV_id],tmp_mean,tmp_variance);
                ggain[PMT_id]->SetTitle(gtitle.Data());
                ggain[PMT_id]->Draw("A*");
                ggain[PMT_id]->Fit("pol1","","",0,12000);
                TF1* fitfuc=(TF1*)ggain[PMT_id]->GetFunction("pol1");
                ratio=fitfuc->GetParameter(1);
                fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],ratio);
                canvas->Print(outPDF);
                delete ggain[PMT_id];
            }
        }
        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);
    }

    return 0;
}

int Get_Gain_Method1_Group3(const char* parentDir,const Int_t testdir_id)
{
    ReadConfigFile(parentDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(parentDir,testdir_id);
    }
    //---------------------
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //---------------------------
    FILE* fp;
    char dir[300];

    TString gtitle;
    TGraph* ggain[CHANNEL_NUM];
    Float_t ratio;
    Float_t tmp_mean[MAXIMUM_DATAPOINTS],tmp_variance[MAXIMUM_DATAPOINTS];
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","Gain Method_1(Group_3)",500,600);
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_gain_method1_group3.dat",dir,VOLTAGE[HV_id]);
        sprintf(outPDF,"%s/%dV_gain_method1_group3.pdf",dir,VOLTAGE[HV_id]);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV gain result(method1,group3):\n",VOLTAGE[HV_id]);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
            if(PMT_INDEX[PMT_id]==0){
                fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
            }
            else{
                for(int Amplitude_id=0;Amplitude_id<GROUP3_FILENO[HV_id];Amplitude_id++){
                    tmp_mean[Amplitude_id]=Data_Dy8[PMT_id][HV_id][GROUP3_FILEINDEX[HV_id][Amplitude_id]-1];
                    tmp_variance[Amplitude_id]=TMath::Power(Sigma_Dy8[PMT_id][HV_id][GROUP3_FILEINDEX[HV_id][Amplitude_id]-1],2);
                }
                gtitle.Form("Gain: Channel_%d, %dV, %s (method1, group3)",PMT_id+1,VOLTAGE[HV_id],PMT_LABEL[PMT_id]);
                ggain[PMT_id]=new TGraph(GROUP3_FILENO[HV_id],tmp_mean,tmp_variance);
                ggain[PMT_id]->SetTitle(gtitle.Data());
                ggain[PMT_id]->Draw("A*");
                ggain[PMT_id]->Fit("pol1","","",0,12000);
                TF1* fitfuc=(TF1*)ggain[PMT_id]->GetFunction("pol1");
                ratio=fitfuc->GetParameter(1);
                fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],ratio);
                canvas->Print(outPDF);
                delete ggain[PMT_id];
            }
        }
        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);
    }

    return 0;
}

int print_channel_diff_M(const Char_t* parentDir,const Int_t ch_num,const Int_t group_id=0)
{
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer[256];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    FILE* fp_out;
    fp_out=fopen("channel_diff.csv","w");
    fprintf(fp_out,"%s channel_%d:\n",parentDir,ch_num);
    fprintf(fp_out,"voltage:\t1000V\t950V\t900V\t850V\t800V\t750V\t700V\n");

    FILE* fp_in;
    char dir[300];
    int tmp_ch;
    char tmp_label[20];
    Float_t tmp_value;
    //--M electron multiplier gain---
    fprintf(fp_out,"M:");
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        if(!group_id){
            sprintf(dir,"%s/%dV/%dV_gain_method1_all.dat",gain_dir,VOLTAGE[HV_id],VOLTAGE[HV_id]);
        }
        else{
            sprintf(dir,"%s/%dV/%dV_gain_method1_group%d.dat",gain_dir,VOLTAGE[HV_id],VOLTAGE[HV_id],group_id);
        }
        fp_in=fopen(dir,"r");
        fgets(buffer,256,fp_in);
        for(int i=1;i<ch_num;i++){
            fgets(buffer,256,fp_in);
        }
        fscanf(fp_in,"%d %s %f\n",&tmp_ch,tmp_label,&tmp_value);
        fprintf(fp_out,"\t%f",tmp_value);
        fclose(fp_in);
    }
    fprintf(fp_out,"\n");

    fclose(fp_out);
    return 0;
}
//-----------------Gain Method 2---------------------
int Calib_Step1(const char* testDir,const Int_t testdir_id,const char* refDir="retest_1",const Int_t refdir_id=1)
{
    ReadConfigFile(testDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(testDir,testdir_id);
    }
    if(REFDATAUPDATE_FLAG==0 || RefDirectory_id!=refdir_id){
        ReadRefIntoDataBuffer(refDir,refdir_id);
    }

    for(int refPMT_id=0;refPMT_id<RefPMT_NO;refPMT_id++){
        for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
            for(int Amplitude_id=0;Amplitude_id<TOTAL_FILENO[HV_id];Amplitude_id++){
                Calib_Ratio[refPMT_id][HV_id][Amplitude_id]=Data_Dy8[refPMT_id][HV_id][Amplitude_id]/Ref_Dy8[refPMT_id][HV_id][Amplitude_id];
            }
        }
    }

    return 0;
}

int Calib_Step2(const Int_t refPMT_channel=1)
{
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                Calibrated_Data_Gain_Step1[PMT_id][Amplitude_id][HV_id]=Data_Gain[PMT_id][Amplitude_id][HV_id]/
                        Calib_Ratio[refPMT_channel-1][HV_id][GAIN_FILEINDEX[HV_id][Amplitude_id]-1];
            }
        }
    }

    return 0;
}

int Calib_Step3()
{
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                Calibrated_Data_Gain_Step2[PMT_id][Amplitude_id][HV_id]=Calibrated_Data_Gain_Step1[PMT_id][Amplitude_id][HV_id]/
                        CHANNEL_DIFF[PMT_id];
            }
        }
    }

    return 0;
}

int print_calib_result(const char* testDir,const Int_t testdir_id,const char* refDir="retest_1",const Int_t refdir_id=1,const Int_t refPMT_channel=1)
{
    Calib_Step1(testDir,testdir_id,refDir,refdir_id);
    Calib_Step2(refPMT_channel);
    Calib_Step3();
    //-----------------------------
    char analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(analysis_dir,"%s/analysis",testDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //---------------------------
    FILE* fp;
    char dir[300],outDAT[256];
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
            sprintf(outDAT,"%s/%dV_gain_method2_amp%d_step1_ref%d.dat",dir,VOLTAGE[HV_id],Amplitude_id+1,refPMT_channel);
            fp=fopen(outDAT,"w");
            if(!fp)
            {
                printf("error: can't create %s\n",outDAT);
                return -1;
            }
            sprintf(buffer1,"%dV gain result(method2,amplitude%d,step1,ref%d):\n",VOLTAGE[HV_id],Amplitude_id+1,refPMT_channel);
            fputs(buffer1,fp);
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                if(PMT_INDEX[PMT_id]==0){
                    fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
                }
                else{
                    fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],Calibrated_Data_Gain_Step1[PMT_id][Amplitude_id][HV_id]);
                }
            }
            fclose(fp);
        }

        for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
            sprintf(outDAT,"%s/%dV_gain_method2_amp%d_step2_ref%d.dat",dir,VOLTAGE[HV_id],Amplitude_id+1,refPMT_channel);
            fp=fopen(outDAT,"w");
            if(!fp)
            {
                printf("error: can't create %s\n",outDAT);
                return -1;
            }
            sprintf(buffer1,"%dV gain result(method2,amplitude%d,step2,ref%d):\n",VOLTAGE[HV_id],Amplitude_id+1,refPMT_channel);
            fputs(buffer1,fp);
            for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
                if(PMT_INDEX[PMT_id]==0){
                    fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
                }
                else{
                    fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],Calibrated_Data_Gain_Step2[PMT_id][Amplitude_id][HV_id]);
                }
            }
            fclose(fp);
        }

    }

    return 0;
}

int print_channel_diff_datagain(const Char_t* parentDir,const Int_t ch_num,const Int_t refPMT_id=1)
{
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer[256];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    FILE* fp_out;
    fp_out=fopen("channel_diff.csv","w");
    fprintf(fp_out,"%s channel_%d:\n",parentDir,ch_num);
    fprintf(fp_out,"voltage:\t1000V\t950V\t900V\t850V\t800V\t750V\t700V\n");

    FILE* fp_in;
    char dir[300];
    int tmp_ch;
    char tmp_label[20];
    Float_t tmp_value;
    //--calibrated datagain---
    for(int Amplitude_id=0;Amplitude_id<GAIN_FILENO;Amplitude_id++){
        fprintf(fp_out,"Amp%d:",Amplitude_id+1);
        for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
            sprintf(dir,"%s/%dV/%dV_gain_method2_amp%d_step1_ref%d.dat",gain_dir,VOLTAGE[HV_id],VOLTAGE[HV_id],Amplitude_id+1,refPMT_id);
            fp_in=fopen(dir,"r");
            fgets(buffer,256,fp_in);
            for(int i=1;i<ch_num;i++){
                fgets(buffer,256,fp_in);
            }
            fscanf(fp_in,"%d %s %f\n",&tmp_ch,tmp_label,&tmp_value);
            fprintf(fp_out,"\t%f",tmp_value);
            fclose(fp_in);
        }
        fprintf(fp_out,"\n");
    }

    fclose(fp_out);
    return 0;
}

//-----------------Gain Method 3-----------------------
int Get_Gain_Method3_Step1(const char* parentDir,const Int_t testdir_id,const Int_t refPMT_id=1)
{
    ReadConfigFile(parentDir);
    if(DATAUPDATE_FLAG==0 || TestDirectory_id!=testdir_id){
        ReadIntoDataBuffer(parentDir,testdir_id);
    }
    //---------------------
    char root_dir[300],analysis_dir[300],gain_dir[300];
    char buffer1[300];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    if(!(gSystem->OpenDirectory(analysis_dir)))
        gSystem->MakeDirectory(analysis_dir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    if(!(gSystem->OpenDirectory(gain_dir)))
        gSystem->MakeDirectory(gain_dir);
    //-----------------------
    FILE* fp;
    char dir[300];

    TString gtitle;
    TGraph* ggain[CHANNEL_NUM];
    Float_t ratio;
    Float_t tmp_ref[MAXIMUM_DATAPOINTS],tmp_test[MAXIMUM_DATAPOINTS];
    char outDAT[200],outPDF[200];
    TCanvas* canvas=new TCanvas("canvas","Gain Method_3,Step_1",500,600);
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV",gain_dir,VOLTAGE[HV_id]);
        gSystem->MakeDirectory(dir);
        sprintf(outDAT,"%s/%dV_gain_method3_step1_ref%d.dat",dir,VOLTAGE[HV_id],refPMT_id);
        sprintf(outPDF,"%s/%dV_gain_method3_step1_ref%d.pdf",dir,VOLTAGE[HV_id],refPMT_id);
        fp=fopen(outDAT,"w");
        if(!fp)
        {
            printf("error: can't create %s\n",outDAT);
            return -1;
        }
        sprintf(buffer1,"%dV gain result(method3,step1,ref%d):\n",VOLTAGE[HV_id],refPMT_id);
        fputs(buffer1,fp);
        sprintf(buffer1,"%s[",outPDF);
        canvas->Print(buffer1);
        for(int PMT_id=0;PMT_id<CHANNEL_NUM;PMT_id++){
            if(PMT_INDEX[PMT_id]==0){
                fprintf(fp,"%d\tnull\t%f\n",PMT_id+1,0.0);
            }
            else{
                for(int Amplitude_id=0;Amplitude_id<DY58_FILENO[HV_id];Amplitude_id++){
                    tmp_ref[Amplitude_id]=Data_Dy8[refPMT_id-1][HV_id][Amplitude_id];
                    tmp_test[Amplitude_id]=Data_Dy8[PMT_id][HV_id][Amplitude_id];
                }
                gtitle.Form("Gain: Channel_%d, %dV, %s (method3,step1,ref%d)",PMT_id+1,VOLTAGE[HV_id],PMT_LABEL[PMT_id],refPMT_id);
                ggain[PMT_id]=new TGraph(DY58_FILENO[HV_id],tmp_ref,tmp_test);
                ggain[PMT_id]->SetTitle(gtitle.Data());
                ggain[PMT_id]->Draw("A*");
                ggain[PMT_id]->Fit("pol1","","",0,6000);
                TF1* fitfuc=(TF1*)ggain[PMT_id]->GetFunction("pol1");
                ratio=fitfuc->GetParameter(1);
                fprintf(fp,"%d\t%s\t%f\n",PMT_id+1,PMT_LABEL[PMT_id],ratio);
                canvas->Print(outPDF);
                delete ggain[PMT_id];
            }
        }
        sprintf(buffer1,"%s]",outPDF);
        canvas->Print(buffer1);
        fclose(fp);
    }

    delete canvas;
    return 0;
}

//---------print channel diff result---------------------
int print_channel_diff(const Char_t* parentDir,const Int_t ch_num)
{
    char root_dir[300],analysis_dir[300],gain_dir[300],dy58_dir[300];
    char buffer[256];
    sprintf(root_dir,"%s/root_file",parentDir);
    sprintf(analysis_dir,"%s/analysis",parentDir);
    sprintf(gain_dir,"%s/gain",analysis_dir);
    sprintf(dy58_dir,"%s/dy58",analysis_dir);
    FILE* fp_out;
    fp_out=fopen("channel_diff.csv","w");
    fprintf(fp_out,"%s channel_%d:\n",parentDir,ch_num);
    fprintf(fp_out,"voltage:\t1000V\t950V\t900V\t850V\t800V\t750V\t700V\n");

    FILE* fp_in;
    char dir[300];
    int tmp_ch;
    char tmp_label[20];
    Float_t tmp_value;
    //--dy58--
    fprintf(fp_out,"dy58:");
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV/%dV_dy58.dat",dy58_dir,VOLTAGE[HV_id],VOLTAGE[HV_id]);
        fp_in=fopen(dir,"r");
        fgets(buffer,256,fp_in);
        for(int i=1;i<ch_num;i++){
            fgets(buffer,256,fp_in);
        }
        fscanf(fp_in,"%d %s %f\n",&tmp_ch,tmp_label,&tmp_value);
        fprintf(fp_out,"\t%f",tmp_value);
        fclose(fp_in);
    }
    fprintf(fp_out,"\n");
    //--gain method3---
    fprintf(fp_out,"gain_method3:");
    for(int HV_id=0;HV_id<HIGHVOLTAGE_STEP;HV_id++){
        sprintf(dir,"%s/%dV/%dV_gain_method3_step1_ref1.dat",gain_dir,VOLTAGE[HV_id],VOLTAGE[HV_id]);
        fp_in=fopen(dir,"r");
        fgets(buffer,256,fp_in);
        for(int i=1;i<ch_num;i++){
            fgets(buffer,256,fp_in);
        }
        fscanf(fp_in,"%d %s %f\n",&tmp_ch,tmp_label,&tmp_value);
        fprintf(fp_out,"\t%f",tmp_value);
        fclose(fp_in);
    }
    fprintf(fp_out,"\n");

    fclose(fp_out);
    return 0;
}

//------------------------------------------------------
int process(const Char_t* parentDir)
{
    RawDataConv(parentDir);
    ReadConfigFile(parentDir);
    Fit_PedestalData(parentDir);
    Fit_TestingData(parentDir);
    //-------------------------------------------------------------------------------------
    return 0;
}

