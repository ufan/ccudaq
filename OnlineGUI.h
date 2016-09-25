#ifndef OnlineGUI_H
#define OnlineGUI_H

//--------------------------------------------------------------
//  This is the GUI window for the online event display
//
//  Author: Zhou Yong (IMP CAS)
//  Email:  zyong06@gmail.com
//--------------------------------------------------------------

#include <TObjArray.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGListBox.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TGLabel.h>
#include <TRootEmbeddedCanvas.h>
#include <TGTab.h>
#include <TGFSContainer.h>
#include <TGComboBox.h>

enum CommandsId {

  M_CLOSE = 100,

  M_DRAW,
  M_SELECT,
  M_CLEAR_A,
  M_EDIT,
  M_NEXT_A,
  M_PREV_A,

  M_MULTI,

  M_LIST_A
};


class TPad;
class TTimer;

class OnlineGUI : public TGMainFrame {

private:
   enum { kMaxHist = 1000 };
   TGCompositeFrame     *fFA;
   TRootEmbeddedCanvas  *fCanvasA;
   TGListBox            *fListBoxA;
   TGCompositeFrame     *fA1, *fA2, *fA3, *fA4, *fA5;
   TGButton             *fCloseButton;
   TGButton             *fEditButton;
   TGButton             *fDrawButton, *fSelectButton, *fClearButtonA;
   TGButton             *fPrevButtonA, *fNextButtonA;
   TGButton             *fLayoutButton[16];
   TGCheckButton        *fMultiButton;

   TObjArray            *fHisto; //histo container
   Int_t                 position; //current position in array "fHisto"
   Bool_t                flags[kMaxHist]; //true for highlighted histos (ListBoxA)
   TPad                 *pads[16];//addresses of pads in 4x4 matrix
                                 //(display layout for CanvasA)
   Int_t                 histInd[16];//indices of histos drawn in CanvasA
   Int_t                 horLay[4];//horizontal display layout
   Int_t                 verLay[4];//vertical display layout
   Int_t                 cursorIter;//current true position in array "flags"
   Int_t                 xDiv, yDiv;//parameters for CanvasA division in case
                                   //of automatic display layout

  TTimer              *fTimer;
  Bool_t              HandleTimer(Timer* t);

   Bool_t toGreen(Window_t id);
   Bool_t toDefault(Window_t id);
   Bool_t isOverlap();
   Bool_t isLayout();

public:
   OnlineGUI(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~OnlineGUI();
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   Int_t getNextTrueIndex(); //returns -1 in case of no index found
   void resetIter() {cursorIter = -1;}
   void resetFlags() { for (int i = 0; i < kMaxHist; i++) flags[i] = kFALSE; }
   void setCanvasDivision(Int_t number);
   void drawHist();//draws a histo in case of automatic display layout
   void doubleclickedBoxA(const char *text);
   Bool_t importHist(const char *name);
  Bool_t importFromFile(const char *filename);
   void paintHist();//draws a histo in case of user defined display layout

   ClassDef(OnlineGUI,0)
};

#endif
