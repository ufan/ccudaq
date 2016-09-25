#include "OnlineGUI.h"
#include <TCollection.h>
#include <TFile.h>
#include <TKey.h>
#include <TROOT.h>
#include <TGMsgBox.h>
#include <TSystem.h>
#include <TH1.h>
#include <TCanvas.h>

#if !defined S_ISDIR
#define S_ISDIR(m) (((m)&(0170000)) == (0040000))
#endif

ClassImp(OnlineGUI)


Bool_t kSTATUS; //to suppress call of drawHist() when no histo is highlighted
                //in ListBoxA
TCanvas *cA;    //canvas in fCanvasA
Long_t cursA; //current position in ListBoxA  
Int_t totalA;  //total number of entries in ListBoxA 
Int_t count;      //number of user highlighted buttons in Display Layout
Int_t indi[2];    //survice arrays for algorithm which calculates
Int_t indj[2];    //display layout


OnlineGUI::OnlineGUI(const TGWindow *p, UInt_t w, UInt_t h) :
      TGMainFrame(p, w, h)
{
   //--------------------------------------------------------------------
   // Constructor for the class OnlineGUI. Draws a control panel, divides it
   // into sub areas and maps all subwindows
   //--------------------------------------------------------------------


   Int_t i;

   for (i = 0; i < 16; i++) {
      pads[i] = NULL;
      histInd[i] = -1;
   }
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

   // test
   importFromFile("hist4_5.root"); 
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
   number = -1;
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



Bool_t OnlineGUI::toDefault(Window_t id)
{
   //------------------------------------------------------------------
   //
   //      toDefault(Window_t id)
   //
   // Changes the color attributes of a window to default values (gray).
   // Used to change the colors of the buttons in the panel "Display Layout".
   // Does not redraw the button.
   // So to visually change the color needs to be followed by
   // the function NeedRedraw(TGWindow *).
   // Input parameter - ID of the button to which the action must be applied.
   //
   //------------------------------------------------------------------

   SetWindowAttributes_t wattr;
   wattr.fMask = kWABackPixel;
   wattr.fBackgroundPixel = GetDefaultFrameBackground();
   gVirtualX->ChangeWindowAttributes(id, &wattr);
   return kTRUE;

}

Bool_t OnlineGUI::toGreen(Window_t id)
{
   //-------------------------------------------------------------
   //
   //       toGreen(Window_t id)
   //
   // The same as above except changing the color to green.
   //
   //-------------------------------------------------------------

   SetWindowAttributes_t wattr;
   wattr.fMask = kWABackPixel;
   gClient->GetColorByName("green", wattr.fBackgroundPixel);
   gVirtualX->ChangeWindowAttributes(id, &wattr);
   return kTRUE;
}

Bool_t OnlineGUI::isOverlap()
{
   //-------------------------------------------------------------
   //
   //         isOverlap()
   //
   // Checks if a selected display layout overlaps with already existing
   // pads in the canvas cA.
   //
   //-------------------------------------------------------------

   Int_t i, j;
   Int_t tmpIndex;

   for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
         if (verLay[i] && horLay[j]) {
            tmpIndex = 4 * i + j;
            if (histInd[tmpIndex] != -1) return kTRUE;
         }
      }
   }
   return kFALSE;

}

Bool_t OnlineGUI::isLayout()
{
   //-------------------------------------------------------
   //
   //        isLayout()
   //
   // Checks if display layout is set.
   //
   //--------------------------------------------------------

   Int_t i;
   for (i = 0;i < 4;i++) {
      if (horLay[i] != 0) return kTRUE;
      if (verLay[i] != 0) return kTRUE;
   }
   return kFALSE;

}

void OnlineGUI::paintHist()
{
   //--------------------------------------------------------------------
   //
   //              paintHist()
   //
   // Draws a histo in the canvas cA in case of the user defined display layout.
   // The latest display layout has the highest priority. If an overlap
   // with existing pads is detected, they are deleted from cA.
   // Algorithm virtually divides cA into subpads with the matrix layout (4x4).
   // A real pad in which histo will be drawn is constructed from virtual subpads.
   // The number of virtual subpads for the real pad can change in the range 1-16.
   // Arrays histInd[16] and pads[16] keep the "id" of the histo and the
   // address of the real pad
   //
   //            -----------------
   //            |   |   |   |   |
   //            | 1 | 2 | 3 | 4 |
   //            |---|---|---|---|
   //            |   |   |   |   |
   //            | 5 | 6 | 7 | 8 |
   //            |---|---|---|---|
   //            |   |   |   |   |
   //            | 9 | 10| 11| 12|
   //            |---|---|---|---|
   //            |   |   |   |   |
   //            | 13| 14| 15| 16|
   //            -----------------
   //
   //
   // If a histo with id=20 must be drawn in a pad which embraces virtual subpads
   // 1,2,5,6 then
   //              histInd[0] = 20        pads[0] = address of the real pad
   //              histInd[1] = 20        pads[1] = NULL
   //              histInd[4] = 20        pads[4] = NULL
   //              histInd[5] = 20        pads[5] = NULL
   //
   // To search for the pads to be deleted the algorithm uses only array
   // histInd[].
   // Only one of the virtual subpads of the real pad keeps the address
   // to avoid double deleting of the same object.
   // If there is an overlap between the pads which contain the histo with
   // the same "id", then only the latest version is drawn.
   // All the other pads with this histo (even non overlapping with the current
   // one) will be deleted from the canvas.
   // To have several versions of the same histo drawn in the canvas one has
   // to avoid pads overlapping when setting display layout.
   //--------------------------------------------------------------------

   Int_t retval;
   Float_t xmin = 0.0F;
   Float_t xmax = 0.0F;
   Float_t ymin = 0.0F;
   Float_t ymax = 0.0F;
   Int_t i, j, countLocal;
   Int_t ind;
   Int_t tempind;
   TPad *pad;
   const Float_t ratio = 0.25;

   if (!isLayout()) {
      new TGMsgBox(fClient->GetRoot(), this, "Message",
                   "Set Display Layout.",
                   kMBIconExclamation, kMBOk, &retval);
      return;
   }
   resetIter();
   ind = getNextTrueIndex();
   for (i = 0; i < 4; i++) {
      if (horLay[i] && (xmin == 0.0)) xmin = i * ratio + 0.01;
      if (horLay[i] && (xmin != 0.0)) xmax = (i + 1) * ratio - 0.01;
   }
   for (i = 3; i > -1; i--) {
      if (verLay[i] && (ymin == 0.0)) ymin = (3 - i) * ratio + 0.01;
      if (verLay[i] && (ymin != 0.0)) ymax = (4 - i) * ratio - 0.01;
   }
   if (isOverlap()) {
      for (i = 0; i < 16; i++) {
         if (verLay[i/4] && horLay[i%4]) {
            tempind = histInd[i];
            for (j = 0; j < 16; j++) {
               if (histInd[j] == tempind) {
                  histInd[j] = -1;
                  if (pads[j]) {
                     delete pads[j];
                     pads[j] = NULL;
                  }
               }
            }
         }
      }
   }
   pad = new TPad("pad", "pad", xmin, ymin, xmax, ymax);
   pad->SetFillColor(10);
   cA->cd();
   pad->Draw();
   pad->cd();
   if (fHisto->At(ind))((TH1F*) fHisto->At(ind))->Draw();
   cA->cd();
   cA->Modified();
   cA->Update();

   countLocal = 0;
   for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
         if (verLay[i] && horLay[j]) {
            countLocal++;
            histInd[4*i+j] = ind;
            if (countLocal == 1) pads[4*i+j] = pad;
            else pads[4*i+j] = NULL;
         }
      }
   }
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
   Int_t imin, imax;//to calculate display layout
   Int_t jmin, jmax;//to calculate display layout

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

                        // Total number of buttons which can be set in "Display Layout" panel by a user
                        count = 2;
                     }
                     break;

                  case M_CLEAR_A:
                     for (int k = 0; k < 16; k++) {
                        histInd[k] = -1;
                        if (pads[k]) delete pads[k];
                        pads[k] = NULL;
                     }
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
                        if (cursA < 1) cursA = 1;
                        if (cursA > totalA) cursA = totalA;
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
                        if (cursA < 1) cursA = 1;
                        if (cursA > totalA) cursA = totalA;
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

                     // Process the panel "Display Layout"

                     if (parm1 >= 500 && parm1 <= 515 && !fMultiButton->GetState()) {
                        if (count == 2) count = 0;
                        if (count < 2) {
                           toGreen(fLayoutButton[parm1-500]->GetId());
                           verLay[(parm1-500)/4] = 1;
                           horLay[(parm1-500)%4] = 1;
                           fClient->NeedRedraw(fLayoutButton[parm1-500]);
                           indi[count] = (parm1 - 500) / 4;
                           indj[count] = (parm1 - 500) % 4;
                           count++;
                           if (count == 2) {
                              imin = (indi[0] < indi[1]) ? indi[0] : indi[1];
                              imax = (indi[0] > indi[1]) ? indi[0] : indi[1];
                              jmin = (indj[0] < indj[1]) ? indj[0] : indj[1];
                              jmax = (indj[0] > indj[1]) ? indj[0] : indj[1];
                              for (i = 0;i < 4;i++) {
                                 for (j = 0;j < 4;j++) {
                                    if (i >= imin && i <= imax && j >= jmin && j <= jmax) {
                                       toGreen(fLayoutButton[4*i+j]->GetId());
                                       verLay[i] = 1;
                                       horLay[j] = 1;
                                    } else {
                                       toDefault(fLayoutButton[4*i+j]->GetId());
                                       if (i < imin || i > imax) verLay[i] = 0;
                                       if (j < jmin || j > jmax) horLay[j] = 0;
                                    }
                                    fClient->NeedRedraw(fLayoutButton[4*i+j]);
                                 }
                              }
                           }
                           if (count == 1) {
                              for (i = 0;i < 16;i++) {
                                 if (i != (parm1 - 500)) {
                                    toDefault(fLayoutButton[i]->GetId());
                                    if (i / 4 != (parm1 - 500) / 4) verLay[i/4] = 0;
                                    if (i % 4 != (parm1 - 500) % 4) horLay[i%4] = 0;
                                    fClient->NeedRedraw(fLayoutButton[i]);
                                 }
                              }
                           }
                        }
                     }
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
                     for (i = 0; i < 16; i++) {
                        toDefault(fLayoutButton[i]->GetId());
                        fClient->NeedRedraw(fLayoutButton[i]);
                        verLay[i/4] = 0;
                        horLay[i%4] = 0;
                     }
                     count = 0;
                     for (j = 0; j < 16; j++) {
                        pads[j] = NULL;
                        histInd[j] = -1;
                     }
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
                     } else
                        flags[parm2-1] = !flags[parm2-1];
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

   paintHist(); //User defined display layout
   // Total number of buttons which can be set in "Display Layout" panel by a user
   count = 2;
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
