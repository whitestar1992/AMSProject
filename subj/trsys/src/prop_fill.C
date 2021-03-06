//#define __HAS_TESTPROP__
//#define __HAS_TESTFIT__
#define __HAS_AMS_OFFICE_LIBS__
#include <CPPLibs/CPPLibs.h>
#include <ROOTLibs/ROOTLibs.h>
#include <TRACKLibs/TRACKLibs.h>

#include "/ams_home/hchou/AMSCore/prod/18Feb27/src/ClassDef.h"

using namespace std;

int main(int argc, char * argv[]) {
    using namespace MGROOT;
    using namespace TrackSys;
    MGROOT::LoadDefaultEnvironment();
    Hist::AddDirectory();

    MGConfig::JobOpt opt(argc, argv);

    TChain * dst = new TChain("data");
    for (auto&& file : opt.flist()) dst->Add(file.c_str());

    LIST * fList = new LIST;
    G4MC * fG4mc = (opt.mode() == MGConfig::JobOpt::MODE::MC ) ? new G4MC : nullptr;
    RTI  * fRti  = (opt.mode() == MGConfig::JobOpt::MODE::ISS) ? new RTI  : nullptr;
    TRG  * fTrg  = new TRG ;
    TOF  * fTof  = new TOF ;
    ACC  * fAcc  = new ACC ;
    TRK  * fTrk  = new TRK ;
    TRD  * fTrd  = new TRD ;
    RICH * fRich = new RICH;
    ECAL * fEcal = new ECAL;

    dst->SetBranchAddress("list", &fList);
    if (opt.mode() == MGConfig::JobOpt::MODE::MC)
        dst->SetBranchAddress("g4mc", &fG4mc);
    if (opt.mode() == MGConfig::JobOpt::MODE::ISS)
        dst->SetBranchAddress("rti",  &fRti);
    dst->SetBranchAddress("trg",  &fTrg);
    dst->SetBranchAddress("tof",  &fTof);
    dst->SetBranchAddress("acc",  &fAcc);
    dst->SetBranchAddress("trk",  &fTrk);
    dst->SetBranchAddress("trd",  &fTrd);
    dst->SetBranchAddress("rich", &fRich);
    dst->SetBranchAddress("ecal", &fEcal);
    
    //---------------------------------------------------------------//
    //---------------------------------------------------------------//
    //---------------------------------------------------------------//
    //PhyArg::SetOpt(true, false);
    PhyArg::SetOpt(true, true);
    Int_t laySat = 1;
    Int_t layEnd = 2;
    
    TFile * ofle = new TFile(Form("%s/prop_fill%04ld.root", opt.opath().c_str(), opt.gi()), "RECREATE");
    
    Axis AXmom("Momentum [GeV]", 200, 0.5, 800., AxisScale::kLog);
    
    Axis AXcxy("Coo [cm]", 260, -65., 65.);
    Hist * hEvt = Hist::New("hEvt", "hEvt", HistAxis(AXcxy, AXcxy));

    // Prop
    Axis AXcoo("Residual [cm * p#beta/Q * L^-1]", 800, -0.25, 0.25);
    Axis AXagl("Residual [p#beta/Q]", 800, -0.25, 0.25);
    Axis AXels("Eloss [MeV * #beta^{2}/Q^{2}]", 1200, 0.2, 18);
    
    Hist* hMcx = Hist::New("hMcx", "hMcx", HistAxis(AXmom, AXcoo)); // (TH2D) MC: residual x
    Hist* hMcy = Hist::New("hMcy", "hMcy", HistAxis(AXmom, AXcoo)); // (TH2D) MC: residual y
    Hist* hTcx = Hist::New("hTcx", "hTcx", HistAxis(AXmom, AXcoo)); // (TH2D) ToyMC: residual x
    Hist* hTcy = Hist::New("hTcy", "hTcy", HistAxis(AXmom, AXcoo)); // (TH2D) ToyMC: residual y

    Hist* hMux = Hist::New("hMux", "hMux", HistAxis(AXmom, AXagl)); // (TH2D) MC: cosine angle x
    Hist* hMuy = Hist::New("hMuy", "hMuy", HistAxis(AXmom, AXagl)); // (TH2D) MC: cosine angle y
    Hist* hTux = Hist::New("hTux", "hTux", HistAxis(AXmom, AXagl)); // (TH2D) ToyMC: cosine angle x
    Hist* hTuy = Hist::New("hTuy", "hTuy", HistAxis(AXmom, AXagl)); // (TH2D) ToyMC: cosine angle y
    
    Hist* hMcux = Hist::New("hMcux", "hMcux", HistAxis(AXcoo, AXagl)); // (TH2D) MC: residual x vs. cosine angle x
    Hist* hMcuy = Hist::New("hMcuy", "hMcuy", HistAxis(AXcoo, AXagl)); // (TH2D) MC: residual y vs. cosine angle y
    Hist* hTcux = Hist::New("hTcux", "hTcux", HistAxis(AXcoo, AXagl)); // (TH2D) ToyMC: residual x vs. cosine angle x
    Hist* hTcuy = Hist::New("hTcuy", "hTcuy", HistAxis(AXcoo, AXagl)); // (TH2D) ToyMC: residual y vs. cosine angle y

    Hist* hMee = Hist::New("hMee", "hMee", HistAxis(AXmom, AXels)); // (TH2D) MC: kinetic energy difference
    Hist* hTee = Hist::New("hTee", "hTee", HistAxis(AXmom, AXels)); // (TH2D) ToyMC: kinetic energy difference
    
    Axis AXcoo2("Residual [cm * p#beta/Q * L^-1]", 2000, -0.5, 0.5);
    Axis AXagl2("Residual [p#beta/Q]", 2000, -0.5, 0.5);
    Hist* hMcx2 = Hist::New("hMcx2", "hMcx2", HistAxis(AXcoo2)); // (TH1D) MC: residual x
    Hist* hMcy2 = Hist::New("hMcy2", "hMcy2", HistAxis(AXcoo2)); // (TH1D) MC: residual y
    Hist* hTcx2 = Hist::New("hTcx2", "hTcx2", HistAxis(AXcoo2)); // (TH1D) ToyMC: residual x
    Hist* hTcy2 = Hist::New("hTcy2", "hTcy2", HistAxis(AXcoo2)); // (TH1D) ToyMC: residual y
    
    Hist* hMux2 = Hist::New("hMux2", "hMux2", HistAxis(AXagl2)); // (TH1D) MC: cosine angle x
    Hist* hMuy2 = Hist::New("hMuy2", "hMuy2", HistAxis(AXagl2)); // (TH1D) MC: cosine angle y
    Hist* hTux2 = Hist::New("hTux2", "hTux2", HistAxis(AXagl2)); // (TH1D) ToyMC: cosine angle x
    Hist* hTuy2 = Hist::New("hTuy2", "hTuy2", HistAxis(AXagl2)); // (TH1D) ToyMC: cosine angle y

    Long64_t printRate = static_cast<Long64_t>(0.04*dst->GetEntries());
    std::cout << Form("\n==== Totally Entries %lld ====\n", dst->GetEntries());
    for (Long64_t entry = 0; entry < dst->GetEntries(); ++entry) {
        if (entry%printRate==0) COUT("Entry %lld/%lld\n", entry, dst->GetEntries());
        dst->GetEntry(entry);

        TrackInfo& track = fTrk->track;
       
        // Geometry (TOF)
        if (fTof->numOfBetaH != 1) continue;
        if (!fTof->statusBetaH) continue;
        if (fTof->betaHPatt != 15) continue;
        
        // Geometry (TRD)
        if (fTrd->numOfTrack != 1 && fTrd->numOfHTrack != 1) continue;
        if (!fTrd->statusKCls[0]) continue;
        if (fTrd->LLRnhit[0] < 10) continue;
        
        // Geometry (ACC)
        if (fAcc->clusters.size() != 0) continue;
        
        // Down-going
        if (fTof->betaH < 0.) continue;

        // Charge
        if (fTof->Qall < 0.8 || fTof->Qall > 1.3) continue;
        if (track.QIn < 0.8 || track.QIn > 1.3) continue;

        // TOF
        if (fTof->normChisqT > 10.) continue;
        if (fTof->normChisqC > 10.) continue;
        
        if (fTof->numOfInTimeCls > 4) continue;
        if ((fTof->extClsN[0]+fTof->extClsN[1]) > 0 || 
            (fTof->extClsN[2]+fTof->extClsN[3]) > 1) continue; 
        
        // No Interaction
        if (fG4mc->primVtx.status && fG4mc->primVtx.coo[2] > -100.) continue;

        // REC hit
        HitTRKInfo* rec[9]; std::fill_n(rec, 9, nullptr);
        for (auto&& hit : track.hits) { rec[hit.layJ-1] = &hit; }

        // MC hit
        HitTRKMCInfo* mch[9]; std::fill_n(mch, 9, nullptr);
        for (auto&& hit : fG4mc->primPart.hits) { mch[hit.layJ-1] = &hit; }
        
        SegPARTMCInfo* mcs[9]; std::fill_n(mcs, 9, nullptr);
        for (auto&& seg : fG4mc->primPart.segs) { mcs[seg.lay-1] = &seg; }

        Bool_t hasSat = (rec[laySat-1] && mch[laySat-1] && mcs[laySat-1]);
        Bool_t hasEnd = (rec[layEnd-1] && mch[layEnd-1] && mcs[layEnd-1]);

        if (!hasSat || !hasEnd) continue;
        SegPARTMCInfo* mcsU = mcs[laySat-1];
        SegPARTMCInfo* mcsL = mcs[layEnd-1];

        PhySt part(PartType::Proton);
        part.set_state_with_cos(
            mcsU->coo[0], mcsU->coo[1], mcsU->coo[2],
            mcsU->dir[0], mcsU->dir[1], mcsU->dir[2]
        );
        part.set_mom(mcsU->mom);
        Double_t mc_mom = part.mom();
        
        PhySt ppst(part);  // Particle Status
        PropMgnt::PropToZ(mcsL->coo[2], ppst); // Propagate to Z with magnetic field
        Double_t len = ppst.arg().len(); // Delta Z
        Double_t nrl = ppst.arg().nrl(); // Number of Radiator Length [1]
        Double_t ela = ppst.arg().ela(); // Electron Abundance [mol/cm^2]
        SVecD<3> refc = ppst.c(); // coord
        SVecD<3> refu = ppst.u(); // cosine angle
        
        const Double_t GeV2MeV = 1.0e3;
        Double_t scl_eloss = (part.bta() * part.bta()) / (part.chrg() * part.chrg()) / ela;       // normalized factor (energy loss)
        Double_t scl_mscat = (part.mom() * part.bta()) / std::fabs(part.chrg()) / std::sqrt(nrl); // normalized factor (multiple-scattering)
        
        ppst = part;
        PropMgnt::PropToZWithMC(mcsL->coo[2], ppst);
        Double_t msqr = part.mass() * part.mass();
        Double_t mc_resc[2] = { mcsL->coo[0] - refc(0), mcsL->coo[1] - refc(1) }; // MC: residual xy [cm]
        Double_t mc_resu[2] = { mcsL->dir[0] - refu(0), mcsL->dir[1] - refu(1) }; // MC: cosine angle xy [1]
        Double_t mc_elsm    = GeV2MeV * (std::sqrt(mcsU->mom*mcsU->mom+msqr) - std::sqrt(mcsL->mom*mcsL->mom+msqr)); // MC: kinetic energy difference [GeV]
        Double_t tm_resc[2] = { ppst.cx() - refc(0), ppst.cy() - refc(1) };       // ToyMC: residual xy [cm]
        Double_t tm_resu[2] = { ppst.ux() - refu(0), ppst.uy() - refu(1) };       // ToyMC: cosine angle xy [1]
        Double_t tm_elsm    = GeV2MeV * (std::sqrt(part.mom()*part.mom()+msqr) - std::sqrt(ppst.mom()*ppst.mom()+msqr));  // ToyMC: kinetic energy difference [GeV]

        //COUT("RES %14.8f %14.8f\n", mc_resc[0], tm_resc[0]);
        //COUT("ELS %14.8f %14.8f\n", mc_elsm, tm_elsm);

        hMcx->fillH2D(mc_mom, scl_mscat * mc_resc[0] / len);
        hMcy->fillH2D(mc_mom, scl_mscat * mc_resc[1] / len);
        hMux->fillH2D(mc_mom, scl_mscat * mc_resu[0]);
        hMuy->fillH2D(mc_mom, scl_mscat * mc_resu[1]);
        hMee->fillH2D(mc_mom, scl_eloss * mc_elsm);
        hTcx->fillH2D(mc_mom, scl_mscat * tm_resc[0] / len);
        hTcy->fillH2D(mc_mom, scl_mscat * tm_resc[1] / len);
        hTux->fillH2D(mc_mom, scl_mscat * tm_resu[0]);
        hTuy->fillH2D(mc_mom, scl_mscat * tm_resu[1]);
        hTee->fillH2D(mc_mom, scl_eloss * tm_elsm);
        
        if (mc_mom > 5.0) hMcux->fillH2D(scl_mscat * mc_resc[0] / len, scl_mscat * mc_resu[0]);
        if (mc_mom > 5.0) hMcuy->fillH2D(scl_mscat * mc_resc[1] / len, scl_mscat * mc_resu[1]);
        if (mc_mom > 5.0) hTcux->fillH2D(scl_mscat * tm_resc[0] / len, scl_mscat * tm_resu[0]);
        if (mc_mom > 5.0) hTcuy->fillH2D(scl_mscat * tm_resc[1] / len, scl_mscat * tm_resu[1]);
        
        if (mc_mom > 5.0) hMcx2->fillH1D(scl_mscat * mc_resc[0] / len);
        if (mc_mom > 5.0) hMcy2->fillH1D(scl_mscat * mc_resc[1] / len);
        if (mc_mom > 5.0) hMux2->fillH1D(scl_mscat * mc_resu[0]);
        if (mc_mom > 5.0) hMuy2->fillH1D(scl_mscat * mc_resu[1]);
        if (mc_mom > 5.0) hTcx2->fillH1D(scl_mscat * tm_resc[0] / len);
        if (mc_mom > 5.0) hTcy2->fillH1D(scl_mscat * tm_resc[1] / len);
        if (mc_mom > 5.0) hTux2->fillH1D(scl_mscat * tm_resu[0]);
        if (mc_mom > 5.0) hTuy2->fillH1D(scl_mscat * tm_resu[1]);
        
        Double_t cx = 0.5 * (mcsU->coo[0] + mcsL->coo[0]);
        Double_t cy = 0.5 * (mcsU->coo[1] + mcsL->coo[1]);
        hEvt->fill(cx, cy);
    }

    ofle->Write();
    ofle->Close();

    //---------------------------------------------------------------//
    //---------------------------------------------------------------//
    //---------------------------------------------------------------//
    if (fList) { delete fList; fList = nullptr; }
    if (fG4mc) { delete fG4mc; fG4mc = nullptr; }
    if (fRti ) { delete fRti ; fRti  = nullptr; }
    if (fTrg ) { delete fTrg ; fTrg  = nullptr; }
    if (fTof ) { delete fTof ; fTof  = nullptr; }
    if (fAcc ) { delete fAcc ; fAcc  = nullptr; }
    if (fTrk ) { delete fTrk ; fTrk  = nullptr; }
    if (fTrd ) { delete fTrd ; fTrd  = nullptr; }
    if (fRich) { delete fRich; fRich = nullptr; }
    if (fEcal) { delete fEcal; fEcal = nullptr; }

    return 0;
}
