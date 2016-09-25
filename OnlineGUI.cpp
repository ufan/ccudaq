#include "OnlineGUI.h"
#include <TCollection.h>
#include <TFile.h>
#include <TKey.h>
#include <TROOT.h>
#include <TGMsgBox.h>
#include <TSystem.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TTime.h>

ClassImp(OnlineGUI)


Bool_t kSTATUS; //to suppress call of drawHist() when no histo is highlighted
                //in ListBoxA
TCanvas *cA;    //canvas in fCanvasA
Long_t cursA; //current position in ListBoxA  
Int_t totalA;  //total number of entries in ListBoxA 
Int_t indi[2];    //survice arrays for algorithm which calculates
Int_t indj[2];    //display layout


OnlineGUI::OnlineGUI(const TGWindow *p, UInt_t w, UInt_t h) :
      TGMainFrame(p, w, h)
{
   //--------------------------------------------------------------------
   // Constructor for the class OnlineGUI. Draws a control panel, divides it
   // into sub areas and maps all subwindows
   //--------------------------------------------------------------------
   int i;

   fHisto = new TObjArray(kMaxHist);
   position = 0;
   totalA = 0;
   resetIter();
   resetFlags();


   /*

                                --------------------------
                                |                        |
                                |                        |
                                -------------fF0----------
                                |                        |
                                --------------------------
                               /                         \
                       ---------------               -------------------
                       |      |      |               |Import \ Scan    |
                       |     fFA     |               |        ---------|
                       |      |      |               |                 |
                       |      |      |               |      fTab       |
                       ---------------               -------------------

   */


   fFA = new TGCompositeFrame(this, 200, 20, kHorizontalFrame);
   AddFrame(fFA, new TGLayoutHints(kLHintsTop | kLHintsLeft));

   /*
   //-------------------------------------------------------------------
   //
   //  Panel A (main presentation canvas, list box with the names of
   //           histogrammes, control buttons, display layout, close button)
   //
   //-------------------------------------------------------------------
   //
   //
   //               --------------------
   //               |      |           |
   //               |     fFA          |
   //               |      |           |
   //               --------------------
   //              /                    \
   //   ---------------            ----------------
   //   |             |            |              |
   //   |             |            |----fA1-------|
   //   |  fCanvasA   |            |              |
   //   |             |            |              |
   //   ---------------            ----------------
   //                              /               \
   //                             /                 \
   //                      ---------------       ----------------
   //                      |             |       |      |       |
   //              Control |     fA2     |       |      |       |
   //              Buttons |             |       |     fA3      |
   //                      |Matrix Layout|       |      |       |
   //                      |             |       |      |       |
   //                      ---------------       ----------------
   //                                           /               \
   //                                          /                 \  fA4
   //                                 --------------         --------------
   //                                 |            |         |fMultiButton|
   //                                 |            |         |------------|
   //                                 | fListBoxA  |         |fPrevButtonA|
   //                                 |            |         |------------|
   //                                 |            |         |fNextButtonA|
   //                                 --------------         |------------|
   //                                                        |fA5(Display |
   //                                                        |    Layout) |
   //                                                        |------------|
   //                                                        |fCloseButton|
   //                                                        --------------
   //
   //------------------------------------------------------------------------
   */
   // Canvas
   fCanvasA = new TRootEmbeddedCanvas("canvasA", fFA, 400, 400);
   fFA->AddFrame(fCanvasA, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));
   cA = fCanvasA->GetCanvas();
   //cA->SetCanvasSize(396,396);
   cA->SetFillColor(10);

   // Control Buttons
   fA1 = new TGCompositeFrame(fFA, 100, 20, kVerticalFrame);
   fFA->AddFrame(fA1, new TGLayoutHints(kLHintsTop | kLHintsLeft));


   fA2 = new TGGroupFrame(fA1, "Control Buttons", kHorizontalFrame);
   fA2->SetLayoutManager(new TGMatrixLayout(fA2, 0, 3, 8));
   fA1->AddFrame(fA2, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));


   fDrawButton = new TGTextButton(fA2, "Draw", M_DRAW);
   fDrawButton->Associate(this);
   fDrawButton->Resize(65, fDrawButton->GetDefaultHeight());
   fA2->AddFrame(fDrawButton);


   fSelectButton = new TGTextButton(fA2, "Select All", M_SELECT);
   fSelectButton->Associate(this);
   fSelectButton->Resize(65, fSelectButton->GetDefaultHeight());
   fA2->AddFrame(fSelectButton);


   fClearButtonA = new TGTextButton(fA2, "Clear", M_CLEAR_A);
   fClearButtonA->Associate(this);
   fClearButtonA->Resize(65, fClearButtonA->GetDefaultHeight());
   fA2->AddFrame(fClearButtonA);


   fEditButton = new TGTextButton(fA2, "Edit Pic", M_EDIT);
   fEditButton->Associate(this);
   fEditButton->Resize(65, fEditButton->GetDefaultHeight());
   fA2->AddFrame(fEditButton);

   // Histogram list
   fA3 = new TGCompositeFrame(fA1, 200, 50, kHorizontalFrame);
   fA1->AddFrame(fA3, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));


   fListBoxA = new TGListBox(fA3, M_LIST_A);
   fListBoxA->Associate(this);
   fA3->AddFrame(fListBoxA, new TGLayoutHints(kLHintsTop | kLHintsLeft));
   fListBoxA->Resize(140, 305);
   fListBoxA->Connect("DoubleClicked(char*)", "OnlineGUI", this, "doubleclickedBoxA(char*)");

   // Selection Button group
   fA4 = new TGCompositeFrame(fA3, 100, 20, kVerticalFrame);
   fA3->AddFrame(fA4, new TGLayoutHints(kLHintsTop | kLHintsLeft));


   fMultiButton = new TGCheckButton(fA4, "Multiple selection", M_MULTI);
   fMultiButton->Associate(this);
   fMultiButton->SetState(kButtonUp);
   fA4->AddFrame(fMultiButton, new TGLayoutHints(kLHintsTop | kLHintsLeft,
                 5, 5, 5, 5));


   fPrevButtonA = new TGTextButton(fA4, "Previous", M_PREV_A);
   fPrevButtonA->Associate(this);
   fA4->AddFrame(fPrevButtonA, new TGLayoutHints(kLHintsLeft | kLHintsTop,
                 10, 5, 5, 5));


   fNextButtonA = new TGTextButton(fA4, "   Next   ", M_NEXT_A);
   fNextButtonA->Associate(this);
   fA4->AddFrame(fNextButtonA, new TGLayoutHints(kLHintsLeft | kLHintsTop,
                 10, 5, 5, 5));

   // Display layout Button Group
   fA5 = new TGGroupFrame(fA4, "Display Layout", kVerticalFrame);
   fA4->AddFrame(fA5, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));
   fA5->SetLayoutManager(new TGMatrixLayout(fA5, 0, 4, kVerticalFrame));


   for (i = 0;i < 16;i++) {
      fLayoutButton[i] = new TGTextButton(fA5, "     ", 500 + i);
      fLayoutButton[i]->Associate(this);
      fA5->AddFrame(fLayoutButton[i], new TGLayoutHints(kLHintsTop | kLHintsLeft,
                    5, 5, 5, 5));
   }

   // Close Window Button
   fCloseButton = new TGTextButton(fA4, "Close Window", M_CLOSE);
   fCloseButton->Associate(this);
   SetWindowAttributes_t wattr;
   wattr.fMask = kWABackPixel;
   gClient->GetColorByName("red", wattr.fBackgroundPixel);
   gVirtualX->ChangeWindowAttributes(fCloseButton->GetId(), &wattr);
   fA4->AddFrame(fCloseButton, new TGLayoutHints(kLHintsTop | kLHintsLeft |
                 kLHintsCenterX, 0, 0, 40, 5));


   MapSubwindows();

   SetWindowName("Online Display");

   Resize(GetDefaultSize());
   MapWindow();

   // set timer
   fTimer = new TTimer(this ,1500);
   fTimer->TurnOn();
  
}

OnlineGUI::~OnlineGUI()
{
   //----------------------------------------------------------
   //
   //    Destructor for the class OnlineGUI
   //
   //----------------------------------------------------------

   delete fCloseButton;

   delete fDrawButton;
   delete fSelectButton;
   delete fEditButton;
   delete fClearButtonA;

   delete fPrevButtonA;
   delete fNextButtonA;
   delete fMultiButton;
   delete fLayoutButton[0];
   delete fLayoutButton[1];
   delete fLayoutButton[2];
   delete fLayoutButton[3];
   delete fLayoutButton[4];
   delete fLayoutButton[5];
   delete fLayoutButton[6];
   delete fLayoutButton[7];
   delete fLayoutButton[8];
   delete fLayoutButton[9];
   delete fLayoutButton[10];
   delete fLayoutButton[11];
   delete fLayoutButton[12];
   delete fLayoutButton[13];
   delete fLayoutButton[14];
   delete fLayoutButton[15];

   delete fListBoxA;

   delete fA5;// display layout frame
   delete fA4;// selection button frame
   delete fA3;// hist list frame

   delete fA2;// control botton frame

   delete fCanvasA;
   delete fA1;

   delete fFA;// global frame

   delete fTimer;
}

void OnlineGUI::CloseWindow()
{
   //-----------------------------------------------------
   //
   //           CloseWindow()
   //
   //     Closes the panel "Histogram Viewer"
   //
   //-----------------------------------------------------

   delete this;
   gApplication->Terminate(0);

}

Bool_t OnlineGUI::importHist(const char *name)
{
   //------------------------------------------------------------------
   //
   //               importHist(const char *name)
   //
   // Allows to import an existing histogram from the memory
   // (needs histo name as an input parameter).
   // This function is not used in GUI at the moment, one can call it from
   // the interpreter.
   //
   // Example:
   //           gui = new OnlineGUI(gClient->GetRoot(),1,1);
   //           TH1F *hist = new TH1F("myhisto","bla-bla-bla",100,0,100);
   //           {
   //           ...
   //           hist->Fill(x);
   //           ...
   //           }
   //           gui->importHist(hist->GetName());
   //
   //-------------------------------------------------------------------

   TH1F *h;
   h = (TH1F*) gROOT->FindObject(name);
   if (!h) return kFALSE;
   if (position == kMaxHist) return kFALSE;
   fHisto->AddAt(h, position++);
   fListBoxA->AddEntry(h->GetName(), ++totalA);
   fListBoxA->MapSubwindows();
   fListBoxA->Layout();
   return kTRUE;
}

Bool_t OnlineGUI::importHist(TH1F* h)
{
   if (!h) return kFALSE;
   if (position == kMaxHist) return kFALSE;
   fHisto->AddAt(h, position++);
   fListBoxA->AddEntry(h->GetName(), ++totalA);
   fListBoxA->MapSubwindows();
   fListBoxA->Layout();
   return kTRUE;
}

int OnlineGUI::getNextTrueIndex()
{
   //---------------------------------------------------------------
   //
   //                 getNextTrueIndex()
   //
   // Iterates over array "flags", returns the next "TRUE index".
   // In case of no "TRUE index" found returns -1.
   // "TRUE index" means the index of a histogram currently highlighted in
   // the large list box (ListBoxA).
   //
   //---------------------------------------------------------------

   while (cursorIter < kMaxHist) {
      cursorIter++;
      if (flags[cursorIter]) return cursorIter;
   }
   return -1;
}

void OnlineGUI::setCanvasDivision(Int_t number)
{
   //---------------------------------------------------------------
   //
   //              setCanvasDivision(Int_t number)
   //
   // Calculates the xDiv and yDiv parameters which are used to divide
   // the main canvas (CanvasA) into subpads.
   // This function is used in case of the automatic display layout
   // (checkbutton "Multiple selection" is engaged).
   // The function takes as an input parameter the total number of histogrammes
   // to be displayed.
   // Called from drawHist()
   //
   //---------------------------------------------------------------

   Int_t i, j, k;
   for (i = 1; i < 50; i++) {
      k = 0;
      for (j = i - 1; j <= i; j++) {
         if (number <= i*j) {
            k = j;
            break;
         }
      }
      if (number <= i*k) break;
   }
   xDiv = i;
   yDiv = k;
}

void OnlineGUI::drawHist()
{
   //-----------------------------------------------------------------
   //
   //                drawHist()
   //
   // Draws a set of histogrammes in the canvas cA in case of the automatic
   // display layout (checkbutton "Multiple selection" is engaged).
   // Called when the button "Draw" is clicked.
   //
   //----------------------------------------------------------------

   Int_t number;  //number of highlighted histos in ListBoxA
   Int_t i;
   number = 0;
   resetIter();
   while (getNextTrueIndex() != -1) number++;
   setCanvasDivision(number);
   cA->Clear();
   cA->Divide(xDiv, yDiv);
   resetIter();
   for (i = 0; i < number; i++) {
      cA->cd(i + 1);
      ((TH1F*) fHisto->At(getNextTrueIndex()))->Draw();
   }
   cA->cd();
   cA->Modified();
   cA->Update();
}


void OnlineGUI::paintHist()
{
    cA->cd();
    resetIter();
    ((TH1F*) fHisto->At(getNextTrueIndex()))->Draw();
    cA->Modified();
    cA->Update();

   return;
}

Bool_t OnlineGUI::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   //------------------------------------------------------------------
   //
   //    ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
   //
   // Processes information from all GUI items.
   // Selecting an item usually generates an event with 4 parameters.
   // The first two are packed into msg (first and second bytes).
   // The other two are parm1 and parm2.
   //
   //------------------------------------------------------------------

   Int_t retval; //for class TGMsgBox
   Int_t buttons;//used to construct message panel when Close button is clicked
   Int_t numb;//to update layout of list boxes
   Int_t i, j;

   switch (GET_MSG(msg)) {
      case kC_COMMAND:

         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:

               switch (parm1) {
                  case M_DRAW:
                     if (!totalA) {
                        new TGMsgBox(fClient->GetRoot(), this, "Error message",
                                     "Histo container is empty.",
                                     kMBIconExclamation, kMBOk, &retval);
                        break;
                     }

                     if (!kSTATUS) {
                        new TGMsgBox(fClient->GetRoot(), this, "Help message",
                                     "Highlight the name of the histogram to be displayed.",
                                     kMBIconExclamation, kMBOk, &retval);
                        break;
                     }

                     if (fMultiButton->GetState()) drawHist();//Automatic display layout
                     else {
                        paintHist(); //User defined display layout
                     }
                     break;

                  case M_CLEAR_A:
                     cA->cd();
                     cA->Clear();
                     cA->Update();
                     break;
                  case M_EDIT:
                     cA->cd();
                     cA->EditorBar();
                     break;
                  case M_CLOSE:
                     retval = 0;
                     buttons = 0;
                     buttons |= kMBYes;
                     buttons |= kMBNo;
                     new TGMsgBox(fClient->GetRoot(), this, "Confirm action",
                                  "Close Panel 'Histogram Viewer' ?",
                                  kMBIconQuestion, buttons, &retval);
                     if (retval == 1) CloseWindow();
                     break;
                  case M_PREV_A:

                     // One histo up in ListBoxA

                     if (!fMultiButton->GetState() && (totalA > 0)) {
                        if ((cursA > 0) && (cursA <= totalA)) cursA--;
                        if (cursA < 1) cursA = totalA;
                        fListBoxA->Select(cursA);
                        numb = cursA;
                        while (((--numb) % 14) != 0) { }
                        fListBoxA->SetTopEntry(++numb);
                        SendMessage(this, MK_MSG(kC_COMMAND, kCM_LISTBOX), M_LIST_A, cursA);
                        SendMessage(this, MK_MSG(kC_COMMAND, kCM_BUTTON), M_DRAW, 0);
                     }
                     break;

                  case M_NEXT_A:

                     // One histo down in ListBoxA

                     if (!fMultiButton->GetState() && (totalA > 0)) {
                        if ((cursA > 0) && (cursA <= totalA)) cursA++;
                        if (cursA > totalA) cursA = 1;
                        fListBoxA->Select(cursA);
                        numb = cursA;
                        while (((--numb) % 14) != 0) { }
                        fListBoxA->SetTopEntry(++numb);
                        SendMessage(this, MK_MSG(kC_COMMAND, kCM_LISTBOX), M_LIST_A, cursA);
                        SendMessage(this, MK_MSG(kC_COMMAND, kCM_BUTTON), M_DRAW, 0);
                     }
                     break;

                  case M_SELECT:

                     // "Select All" button is clicked

                     fMultiButton->SetState(kButtonDown);
                     SendMessage(this, MK_MSG(kC_COMMAND, kCM_CHECKBUTTON), M_MULTI, 0);

                     // Call twice SetMultipleSelections(kTRUE), otherwise items in the list box
                     // are not highlighted (though proper functionality remains)

                     fListBoxA->SetMultipleSelections(kTRUE);
                     for (i = 1; i <= totalA; i++) {
                        fListBoxA->Select(i);
                        SendMessage(this, MK_MSG(kC_COMMAND, kCM_LISTBOX), M_LIST_A, i);
                     }
                     break;
                  default:
                     break;
               }

            case kCM_CHECKBUTTON:

               // Multiple selection

               switch (parm1) {
                  case M_MULTI:
                     if (!fListBoxA->GetMultipleSelections()) {
                        if (fListBoxA->GetSelectedEntry())
                           fListBoxA->GetSelectedEntry()->Activate(kFALSE);
                     }
                     fListBoxA->SetMultipleSelections(fMultiButton->GetState());

                     cursA = 0;
                     cA->Clear();
                     cA->Update();

                     resetFlags();
                     kSTATUS = kFALSE;
                     break;

                  default:
                     break;
               }

            case kCM_LISTBOX:

               switch (parm1) {
                  case M_LIST_A:

                     // ListBoxA

                     cursA = parm2; //necessary for "Previous", "Next" buttons in case of
                     //random jumps in list box window

                     if (!fListBoxA->GetMultipleSelections()) {
                        resetFlags();
                        flags[parm2-1] = kTRUE;
                     } else{
                        flags[parm2-1] = !flags[parm2-1];
                     }
                     kSTATUS = kTRUE;
                     break;

                  default:
                     break;
               }

            default:
               break;
         }

      default:
         break;
   }

   return kTRUE;

}

void OnlineGUI::doubleclickedBoxA(const char * /*text*/)
{
   //------------------------------------------------------------------
   //
   //    doubleclickBoxA(const char *)
   //
   // Handle double click events in fListBoxA.
   // Double clicking in the list of histograms will draw the selected
   // histogram.
   //
   //------------------------------------------------------------------
//   if(getNextTrueIndex() != -1)
//    flags[getNextTrueIndex()]= kFALSE;
//   flags[cursA-1] = kTRUE;

   paintHist(); //User defined display layout

}

Bool_t OnlineGUI::importFromFile(const char *filename)
{
   //-------------------------------------------------------------------
   //
   //                 importFromFile(const char *filename)
   //
   // Imports histogrammes from a file with the name "filename".
   // Opens the file, scans it, if finds an object of the class TH1F or TH2F,
   // imports it.
   // All the other objects are ignored.
   // In case of not a ROOT file returns an error message and takes no further
   // action.
   // This function is called when a user doubly clicks on the file icon
   // in the file list view.
   //
   //--------------------------------------------------------------------

   Int_t retval;
   Int_t l;
   TFile *f;
   TH1F *fH;
   f = new TFile(filename);
   if (!f) return kFALSE;
   if (f->IsZombie()) {
      new TGMsgBox(fClient->GetRoot(), this, "Error Message",
                   "You have chosen not a ROOT file. Please, be attentive.",
                   kMBIconExclamation, kMBOk, &retval);
      delete f;
      f = NULL;
      return kFALSE;
   }
   TKey *key;
   TIter it(f->GetListOfKeys());
   while ((key = (TKey*) it())) {
      if (!strcmp(key->GetClassName(), "TH1F") ||
            !strcmp(key->GetClassName(), "TH2F")) {
         fH = (TH1F*) key->ReadObj();
         if (fH && position < kMaxHist) fHisto->AddAt(fH, position++);
      }
   }
   for (l = totalA; l < position; l++) {
      fListBoxA->AddEntry(fHisto->At(l)->GetName(), l + 1);
   }
   fListBoxA->MapSubwindows();
   fListBoxA->Layout();
   totalA = position;
   return kTRUE;
}

Bool_t OnlineGUI::HandleTimer(TTimer* t)
{
  if (!fListBoxA->GetMultipleSelections()){
      cA->cd();
      cA->Modified();
      cA->Update();
  }
  else{
      for(int i=0;i< xDiv;i++){
          for(int j=0;j<yDiv;j++){
              cA->cd(j*xDiv+i+1);
              gPad->Modified();
              gPad->Update();
          }
      }
  }
//  fTimer->Reset();

  return kTRUE;
}

void OnlineGUI::clearHist()
{
    fHisto->Clear();
    fListBoxA->RemoveAll();
}
