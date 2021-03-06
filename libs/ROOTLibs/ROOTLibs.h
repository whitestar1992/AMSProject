#ifndef __ROOTLibs_H__
#define __ROOTLibs_H__

#include <TChain.h>
#include <TTree.h>
#include <TF1.h>
#include <TF2.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>

#include "MGMath.h"

#include "MGStyle.h"
#include "MGStyle.C"

#include "MGCanvas.h"
#include "MGCanvas.C"

#include "MGAxis.h"
#include "MGAxis.C"

#include "MGHist.h"
#include "MGHist.C"

//#include "MGFit.h"
//#include "MGFit.C"


namespace MGROOT {
    void LoadDefaultEnvironment() {
        Style::LoadDefaultEnvironment();
        Hist::LoadDefaultEnvironment();
    }
}


#endif // __ROOTLibs_H__
