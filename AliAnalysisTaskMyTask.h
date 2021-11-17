/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. */
/* See cxx source for full Copyright notice */
/* $Id$ */

#ifndef AliAnalysisTaskMyTask_H
#define AliAnalysisTaskMyTask_H

#include "AliAnalysisTaskSE.h"
//#include "AliUniFlowCorrTask.h"

class TComplex;

class AliAnalysisTaskMyTask : public AliAnalysisTaskSE  
{
    public:
                                AliAnalysisTaskMyTask();
                                AliAnalysisTaskMyTask(const char *name);
        virtual                 ~AliAnalysisTaskMyTask();

        virtual void            UserCreateOutputObjects();
        //virtual void            UserExec(const AliUniFlowCorrTask* task);
        virtual void            UserExec(Option_t* option);

        virtual void            Terminate(Option_t* option);
    
        void FillFlowQVector(const Double_t dWeight, const Double_t dPhi, const Double_t dEta, const Int_t harm);
    
    private:
        static const Int_t      fPIDNumSpecies = 2; // Number of considered species for PID
        static const Int_t      fFlowNumHarmonicsMax = 5; // maximum harmonics length of flow vector array (25)
        static const Int_t      fFlowNumWeightPowersMax = 5; // maximum weight power length of flow vector array (17)
        static const Int_t      fFlowBinNumberEtaSlices = 5; // number of eta bin slices (for correlation study) (32)
    
        Double_t                fEtaSlicesArr[fFlowBinNumberEtaSlices+1]; // array with lower cuts & selection: events
        TComplex                fFlowVecQ[fFlowBinNumberEtaSlices][6][3]; // flow vector array for flow calculation in very narrow delta eta slices (for the correlation study)
        Bool_t                  fCorrQvector; // [kFALSE] fill correlations flag (but using GF and Q-cumulants)
    
        AliAODEvent*            fAOD;           //! input event
        TList*                  fOutputList;    //! output list
        TH1F*                   fHistPt;        //! dummy histogram
        TH1F*                   fHistCorr;
        TH2F*                   fHistEtaPhi;
        TProfile*               fProfileCorr;
        TH1F*                   fHistQvector;      

    
    
        AliAnalysisTaskMyTask(const AliAnalysisTaskMyTask&); // not implemented
        AliAnalysisTaskMyTask& operator=(const AliAnalysisTaskMyTask&); // not implemented

        ClassDef(AliAnalysisTaskMyTask, 1);
};

#endif
