/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* AliAnaysisTaskMyTask
 *
 * empty task which can serve as a starting point for building an analysis
 * as an example, one histogram is filled
 */

#include "TChain.h"
#include "TH1F.h"
#include "TH2.h"
#include "TProfile.h"

#include "TComplex.h"
#include "TList.h"
#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliAODEvent.h"
#include "AliAODInputHandler.h"
#include "AliAnalysisTaskMyTask.h"
//#include "AliUniFlowCorrTask.h"

class AliAnalysisTaskMyTask;    // your analysis class

using namespace std;            // std namespace: so you can do things like 'cout'

ClassImp(AliAnalysisTaskMyTask) // classimp: necessary for root

AliAnalysisTaskMyTask::AliAnalysisTaskMyTask() : AliAnalysisTaskSE(), 
    fAOD(0), fOutputList(0), fHistPt(0), fHistCorr(0), fHistQvector(0), fHistEtaPhi(0), fProfileCorr(0), fEtaSlicesArr{}, fFlowVecQ{}, fCorrQvector{kFALSE}
{
    // default constructor, don't allocate memory here!
    // this is used by root for IO purposes, it needs to remain empty
}
//_____________________________________________________________________________
AliAnalysisTaskMyTask::AliAnalysisTaskMyTask(const char* name) : AliAnalysisTaskSE(name),
    fAOD(0), fOutputList(0), fHistPt(0), fHistCorr(0), fHistQvector(0), fHistEtaPhi(0), fProfileCorr(0),  fEtaSlicesArr{}, fFlowVecQ{}, fCorrQvector{kFALSE}
{
    // constructor
    DefineInput(0, TChain::Class());    // define the input of the analysis: in this case we take a 'chain' of events
                                        // this chain is created by the analysis manager, so no need to worry about it, 
                                        // it does its work automatically
    DefineOutput(1, TList::Class());    // define the ouptut of the analysis: in this case it's a list of histograms 
                                        // you can add more output objects by calling DefineOutput(2, classname::Class())
                                        // if you add more output objects, make sure to call PostData for all of them, and to
                                        // make changes to your AddTask macro!
}
//_____________________________________________________________________________
AliAnalysisTaskMyTask::~AliAnalysisTaskMyTask()
{
    // destructor
    if(fOutputList) {
        delete fOutputList;     // at the end of your task, it is deleted from memory by calling this function
    }
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::UserCreateOutputObjects()
{
    // create output objects
    //
    // this function is called ONCE at the start of your analysis (RUNTIME)
    // here you ceate the histograms that you want to use 
    //
    // the histograms are in this case added to a tlist, this list is in the end saved
    // to an output file
    //
    fOutputList = new TList();          // this is a list which will contain all of your histograms
                                        // at the end of the analysis, the contents of this list are written
                                        // to the output file
    fOutputList->SetOwner(kTRUE);       // memory stuff: the list is owner of all objects it contains and will delete them
                                        // if requested (dont worry about this now)

    // example of a histogram
    fHistPt = new TH1F("fHistPt", "fHistPt", 100, 0, 10);       // create your histogra
    fHistCorr = new TH1F("fHistCorr", "fHistCorr", 100, 0, 10);
    fHistEtaPhi = new TH2F("fHistEtaPhi", "fHistEtaPhi", 100, -TMath::Pi(), TMath::Pi() , 100, 0, 2*TMath::Pi() );
    fProfileCorr = new TProfile("fProfileCorr", "fProfileCorr", 100, 0, 10, -2, 2);
    fHistQvector = new TH1F("fHistQvector", "fHistQvector", 100, -10, 10);

    fOutputList->Add(fHistPt);
    fOutputList->Add(fHistCorr);
    fOutputList->Add(fHistEtaPhi);
    fOutputList->Add(fProfileCorr);
    fOutputList->Add(fHistQvector);

    
    PostData(1, fOutputList);           // postdata will notify the analysis manager of changes / updates to the 
                                        // fOutputList object. the manager will in the end take care of writing your output to file
                                        // so it needs to know what's in the output
}
//_____________________________________________________________________________
//void AliAnalysisTaskMyTask::UserExec(const AliUniFlowCorrTask* const task)
void AliAnalysisTaskMyTask::UserExec(Option_t* option)

{
    // user exec
    // this function is called once for each event
    // the manager will take care of reading the events from file, and with the static function InputEvent() you 
    // have access to the current event. 
    // once you return from the UserExec function, the manager will retrieve the next event from the chain
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());    // get an event (called fAOD) from the input file
                                                        // there's another event format (ESD) which works in a similar wya
                                                        // but is more cpu/memory unfriendly. for now, we'll stick with aod's
    if(!fAOD) return;                                   // if the pointer to the event is empty (getting it failed) skip this event
        // example part: i'll show how to loop over the tracks in an event 
        // and extract some information from them which we'll store in a histogram
    Int_t iTracks(fAOD->GetNumberOfTracks());           // see how many tracks there are in the event
    //cout<<"Event "<<iTracks<<endl;
    //fCorrQvector = kTRUE;
    //task->PrintTask();
    TComplex FlowVector(0,0,kFALSE);
    Int_t harm = 2;
    
    for(Int_t i(0); i < iTracks; i++) {                 // loop ove rall these tracks
        AliAODTrack* track = static_cast<AliAODTrack*>(fAOD->GetTrack(i));         // get a track (type AliAODTrack) from the event
        
        Double_t dPhi = track->Phi();
        Double_t dWeight = track->Pt();
        Double_t dEta = track->Eta();
        Double_t dpT = dWeight;
        
        Double_t dCos = dWeight * TMath::Cos(harm * dPhi);
        Double_t dSin = dWeight * TMath::Sin(harm * dPhi);
        FlowVector += TComplex(dCos,dSin,kFALSE) ;
        
        //if(fCorrQvector) {
            //FillFlowQVector((Double_t) dWeight, (Double_t) dPhi, (Double_t) dEta, (Int_t) task->fiHarm[0]);
            //FillFlowQVector((Double_t) dWeight, (Double_t) dPhi, (Double_t) dEta, 2);
        //}
        //cout<<" --- track:: "<<i<<endl;
        Int_t indicePID =track->GetMostProbablePID();
        
        //if(!track || !track->TestFilterBit(1)) continue;                            // if we failed, skip this track
        
        fHistPt->Fill(dpT);                     // plot the pt value of the track in a histogram
        //fHistCorr->Fill(fFlowVecQ[2][1][1]);
        fHistCorr->Fill( indicePID );
        fHistEtaPhi->Fill(dEta, dPhi);
        fProfileCorr->Fill(dpT, dCos );


    }// continue until all the tracks are processed
    
    Double_t ReFlowVector = (Double_t) FlowVector;

    fHistQvector->Fill(ReFlowVector );
    
    
    PostData(1, fOutputList);                           // stream the results the analysis of this event to
                                                        // the output manager which will take care of writing
                                                        // it to a file
}
//_____________________________________________________________________________
void AliAnalysisTaskMyTask::Terminate(Option_t *)
{
    // terminate
    // called at the END of the analysis (when all events are processed)
}
//_____________________________________________________________________________

// ============================================================================
void AliAnalysisTaskMyTask::FillFlowQVector(const Double_t dWeight, const Double_t dPhi, const Double_t dEta, const Int_t harm)
{
  // Filling Q flow vector with RFP
  for(Int_t iBin(0); iBin < fFlowBinNumberEtaSlices; iBin++){
    if(dEta > fEtaSlicesArr[iBin+1]) continue;
    Double_t dCos = dWeight * TMath::Cos(harm * dPhi);
    Double_t dSin = dWeight * TMath::Sin(harm * dPhi);
    fFlowVecQ[iBin][harm][1] += TComplex(dCos,dSin,kFALSE);

    fFlowVecQ[iBin][0][1] += TComplex(dWeight,(Double_t) 0.0,kFALSE);

    dCos = TMath::Power(dWeight,2);
    fFlowVecQ[iBin][0][2] += TComplex(dCos,(Double_t) 0.0,kFALSE);
    break;
  }
  return;
}
