/**********************************************************
 * Author        : Hsin-Yi Chou
 * Email         : hchou@cern.ch
 * Last modified : 2015-07-22 16:29
 * Filename      : YiProdNtuple.tcc
 * Description   : xxx
 * *******************************************************/
#ifndef __YiProdNtuple_TCC__
#define __YiProdNtuple_TCC__
#include "YiProdNtuple.h"


//---- RecEvent ----//
void RecEvent::init() {
	iBeta       = -1;
	iBetaH      = -1;
	iTrTrack    = -1;
	iEcalShower = -1;
	iTrdTrack   = -1;
	iTrdHTrack  = -1;
	iRichRing   = -1;

	std::fill_n(trackerZJ, 9, 0);
}

bool RecEvent::rebuild(AMSEventR * event) {
	if (event == nullptr) return false;
	fStopwatch.start();
	init();

	/** Particle **/
	bool isParticle = false;
	if (event->NParticle() > 0 && event->pParticle(0) != nullptr) {
		ParticleR * par = event->pParticle(0);

		iBeta       = (par->iBeta()       < 0) ? -1 : par->iBeta()      ;
		iBetaH      = (par->iBetaH()      < 0) ? -1 : par->iBetaH()     ;
		iTrTrack    = (par->iTrTrack()    < 0) ? -1 : par->iTrTrack()   ;
		iEcalShower = (par->iEcalShower() < 0) ? -1 : par->iEcalShower();
		iTrdTrack   = (par->iTrdTrack()   < 0) ? -1 : par->iTrdTrack()  ;
		iTrdHTrack  = (par->iTrdHTrack()  < 0) ? -1 : par->iTrdHTrack() ;
		iRichRing   = (par->iRichRing()   < 0) ? -1 : par->iRichRing()  ;

		for (int it = 0; it < event->NBetaH() && iTrTrack>=0; ++it)
			if (event->pBetaH(it)->iTrTrack() == iTrTrack) iBetaH = it;

		isParticle = true;
	}
	if (!isParticle) { init(); fStopwatch.stop(); return false; }

	// Beta Information
	//float Beta = 0;
	//if      (iBetaH >= 0) Beta = event->pBetaH(iBetaH)->GetBeta();
	//else if (iBeta  >= 0) Beta = event->pBeta(iBeta)->Beta;
	//else                  Beta = 1;

	// Tracker Information
	if (EventBase::checkEventMode(EventBase::ISS))
		for (int layJ = 1; layJ <= 9; layJ++)
			trackerZJ[layJ-1] = TkDBc::Head->GetZlayerAJ(layJ);
	else
		for (int layJ = 1; layJ <= 9; layJ++)
			trackerZJ[layJ-1] = TkDBc::Head->GetZlayerJ(layJ);

	int       TkStID = -1;
	TrTrackR* TkStPar = nullptr;
	if (iTrTrack >= 0) {
		TkStPar = event->pTrTrack(iTrTrack);
		TkStID = TkStPar->iTrTrackPar(1, 3, recEv.trRft(0, 0));
	}
	if (TkStID < 0) { init(); fStopwatch.stop(); return false; }
		
	// ECAL Information
	// pre-selection (ECAL)
	if (TkStID >= 0 && iEcalShower >= 0) {
		EcalShowerR * ecal = event->pEcalShower(iEcalShower);
		AMSPoint EcalPnt(ecal->CofG);
		AMSPoint pnt; AMSDir dir;
		TkStPar->Interpolate(EcalPnt.z(), pnt, dir, TkStID);
		float drPnt = std::hypot(pnt.x() - EcalPnt.x(), pnt.y() - EcalPnt.y());
		float lmtr = 7; // 7 cm
		if (drPnt > lmtr) { iEcalShower = -1; }
	}

	// TRD Information
	// pre-selection (TRD)
	if (TkStID >= 0 && iTrdTrack >= 0) {
		AMSPoint TrdPnt(event->pTrdTrack(iTrdTrack)->Coo);
		AMSPoint pnt; AMSDir dir;
		TkStPar->Interpolate(TrdPnt.z(), pnt, dir, TkStID);
		float drPnt = std::hypot(pnt.x() - TrdPnt.x(), pnt.y() - TrdPnt.y());
		float lmtr = 5; // 5 cm
		if (drPnt > lmtr) { iTrdTrack = -1; }
	}

	if (TkStID >= 0 && iTrdHTrack >= 0) {
		AMSPoint TrdHPnt(event->pTrdHTrack(iTrdHTrack)->Coo);
		AMSPoint pnt; AMSDir dir;
		TkStPar->Interpolate(TrdHPnt.z(), pnt, dir, TkStID);
		float drPnt = std::hypot(pnt.x() - TrdHPnt.x(), pnt.y() - TrdHPnt.y());
		float lmtr = 5; // 5 cm
		if (drPnt > lmtr) { iTrdHTrack = -1; }
	}
	
	fStopwatch.stop();
	return true;
}


//---- EventBase ----//
EventBase::MODE EventBase::eventMode = EventBase::ISS;
EventBase::VERSION EventBase::eventVersion = EventBase::B950;

EventBase::EventBase() {
	isTreeSelf = false;
	tree = 0;
}

EventBase::~EventBase() {
	if (isTreeSelf) deleteTree();
}

inline void EventBase::fill() {
	if (isTreeSelf == false || tree == nullptr) return;
	tree->Fill();
}

inline void EventBase::setTree(const std::string& name, const std::string& title) {
	isTreeSelf = true;
	tree = new TTree(name.c_str(), title.c_str());
}

inline void EventBase::deleteTree() {
	if (tree != 0) delete tree;
	tree = 0;
}


//---- EventList ----//
EventList::EventList() : EventBase() {
}

EventList::~EventList() {
}

void EventList::initEvent() {
	fList.init();
	fG4mc.init();
}

void EventList::setEventTree(TTree * evTree) {
#if Debug == true
	std::cerr << "Debug : Now, EventList::setEventTree()\n";
#endif

	if (evTree == nullptr) setTree("EventList", "AMS-02 Event List");
	else tree = evTree;

	tree->Branch("list", &fList);
	if (checkEventMode(EventBase::MC))
		tree->Branch("g4mc", &fG4mc);
}

void EventList::setEnvironment() {
#if Debug == true
	std::cerr << "Debug : Now, EventList::setEnvironment()\n";
#endif

	if (!checkEventMode(EventBase::ISS)) {
		AMSSetupR::SlowControlR::ReadFromExternalFile = false;
	}
	if (checkEventMode(EventBase::MC)) {
		AMSSetupR::LoadISSMC = false;
	}
}

bool EventList::processEvent(AMSEventR * event, AMSChain * chain) {
	initEvent();
	if (event == nullptr)	return false;
	fStopwatch.start();

	// LIST
	fList.run    = event->Run();
	fList.event  = event->Event();
	fList.entry  = chain->Entry();
	fList.weight = EventList::Weight;

	// G4MC
	if (checkEventMode(EventBase::MC)) {
		MCEventgR * primary = event->GetPrimaryMC();
		if (primary == nullptr) return false;

		fG4mc.beamID = primary->TBl + 1;
		fG4mc.primPart.partID   = primary->Particle;
		fG4mc.primPart.chrg     = primary->Charge;
		fG4mc.primPart.mass     = primary->Mass;
		fG4mc.primPart.beta     = ((primary->Particle==1) ? 1.0 : 1.0/std::hypot(1.0, (primary->Mass/primary->Momentum)));
		fG4mc.primPart.mom      = primary->Momentum;
		fG4mc.primPart.ke       = (std::hypot(primary->Momentum, primary->Mass) - primary->Mass);
		fG4mc.primPart.coo[0]   = primary->Coo[0];
		fG4mc.primPart.coo[1]   = primary->Coo[1];
		fG4mc.primPart.coo[2]   = primary->Coo[2];
		fG4mc.primPart.dir[0]   = primary->Dir[0];
		fG4mc.primPart.dir[1]   = primary->Dir[1];
		fG4mc.primPart.dir[2]   = primary->Dir[2];

        // Only For Primary Particle
        constexpr Int_t Range[2] = { -1000, -1020 };
        constexpr Int_t SiLay[9] = { -1000, -1007, -1008, -1009, -1010, -1011, -1012, -1013, -1018 };
        constexpr Int_t TfLay[4] = { -1004, -1005, -1015, -1016 };
        constexpr Int_t TdLay[2] = { -1001, -1002 };
        constexpr Int_t EcLay[2] = { -1019, -1020 };
		    
        constexpr Float_t kEngTh = 5.0e-2; 
        for (int it = 0; it < event->NMCEventg(); it++) {
		    MCEventgR * mcev = event->pMCEventg(it);
		    if (mcev == nullptr) continue;
            Int_t id = (mcev->trkID == primary->trkID) + (mcev->parentID == primary->trkID) * 2;
		  
            if (id == 0) continue;
            if (id == 1) {
                Short_t dec = -1, lay = -1;
                if (mcev->Nskip > Range[0] || mcev->Nskip < Range[1]) continue;
                for (Short_t il = 0; il < 9 && dec < 0; ++il) if (mcev->Nskip == SiLay[il]) { dec = 0; lay = il+1; break; } // Silicon
                for (Short_t il = 0; il < 4 && dec < 0; ++il) if (mcev->Nskip == TfLay[il]) { dec = 1; lay = il+1; break; } // TOF
                for (Short_t il = 0; il < 2 && dec < 0; ++il) if (mcev->Nskip == TdLay[il]) { dec = 2; lay = il+1; break; } // TRD
                for (Short_t il = 0; il < 2 && dec < 0; ++il) if (mcev->Nskip == EcLay[il]) { dec = 3; lay = il+1; break; } // ECAL
                if (dec < 0 || lay < 0) continue;
                Float_t eta = (fG4mc.primPart.mass / mcev->Momentum);
                Float_t bta = (MGNumc::EqualToZero(fG4mc.primPart.mass) ? 1.0 : 1.0/(1.0+eta*eta));

                SegPARTMCInfo seg;
		        seg.dec = dec;
		        seg.lay = lay;
                seg.bta = bta;
		        seg.mom = mcev->Momentum;
		        seg.coo[0] = mcev->Coo[0];
		        seg.coo[1] = mcev->Coo[1];
		        seg.coo[2] = mcev->Coo[2];
		        seg.dir[0] = mcev->Dir[0];
		        seg.dir[1] = mcev->Dir[1];
		        seg.dir[2] = mcev->Dir[2];

		        fG4mc.primPart.segs.push_back(seg);
            }
            else if (id == 2) {
                Float_t keng = (std::hypot(mcev->Momentum, mcev->Mass) - mcev->Mass);
		        if (keng < kEngTh) continue;
                if (!fG4mc.primVtx.status) {
			        fG4mc.primVtx.status = true;
			        fG4mc.primVtx.coo[0] = mcev->Coo[0];
			        fG4mc.primVtx.coo[1] = mcev->Coo[1];
			        fG4mc.primVtx.coo[2] = mcev->Coo[2];
                }
                if (keng > fG4mc.primVtx.partKe) {
                    fG4mc.primVtx.partId = mcev->Particle;
                    fG4mc.primVtx.partKe = keng;
                }
                fG4mc.primVtx.numOfPart++;
            }
        }
		std::sort(fG4mc.primPart.segs.begin(), fG4mc.primPart.segs.end(), SegPARTMCInfo_sort());
        if (fG4mc.primVtx.numOfPart < 2) fG4mc.primVtx.init();

		for (int icls = 0; icls < event->NTrMCCluster(); icls++) {
			TrMCClusterR * cluster = event->pTrMCCluster(icls);
			if (cluster == nullptr) continue;
			if (cluster->GetGtrkID() != primary->trkID) continue;
            
            int chrg = 0;
            double mass = 0;
            cluster->GetChargeAndMass(chrg, mass);
            double keng = std::hypot(cluster->GetMomentum(), mass) - mass;
            if (keng < kEngTh) continue;

			int tkid = cluster->GetTkId();
			int layJ = TkDBc::Head->GetJFromLayer(std::fabs(cluster->GetTkId()/100));
			AMSPoint coo = cluster->GetXgl();
			
			HitTRKMCInfo hit;
			hit.layJ = layJ;
			hit.tkid = tkid;
			hit.edep = cluster->GetEdep();
			hit.mom  = cluster->GetMomentum();
			hit.coo[0] = coo[0];
			hit.coo[1] = coo[1];
			hit.coo[2] = coo[2];

            for (auto&& seg : fG4mc.primPart.segs) {
                if (seg.dec != 0 || seg.lay != hit.layJ) continue;
                float dz = (hit.coo[2] - seg.coo[2]);
                float tx = (seg.dir[0] / seg.dir[2]);
                float ty = (seg.dir[1] / seg.dir[2]);
                float cx = seg.coo[0] + dz * tx;
                float cy = seg.coo[1] + dz * ty;
                hit.smr[0] = (hit.coo[0] - cx);
                hit.smr[1] = (hit.coo[1] - cy);
                hit.coo[0] = cx;
                hit.coo[1] = cy;
           
                AMSPoint loc(hit.coo[0], hit.coo[1], hit.coo[2]);
                TkSens tksens(loc, EventBase::checkEventMode(EventBase::MC));
                double xloc = (!tksens.LadFound() || MGNumc::Compare(std::fabs(tksens.GetImpactPointX()), 0.5) > 0) ? -1.0 : tksens.GetImpactPointX();
                double yloc = (!tksens.LadFound() || MGNumc::Compare(std::fabs(tksens.GetImpactPointY()), 0.5) > 0) ? -1.0 : tksens.GetImpactPointY();
                hit.loc[0] = xloc;
                hit.loc[1] = yloc;
            }
			
			fG4mc.primPart.hits.push_back(hit);
		}
		std::sort(fG4mc.primPart.hits.begin(), fG4mc.primPart.hits.end(), HitTRKMCInfo_sort());
	}

	fStopwatch.stop();
	return selectEvent(event);
}

bool EventList::selectEvent(AMSEventR * event) {
	return true;
}


//---- EventRti ----//
EventRti::EventRti() : EventBase() {
}

EventRti::~EventRti() {
}

void EventRti::initEvent() {
	fRti.init();
}

void EventRti::setEventTree(TTree * evTree) {
#if Debug == true
	std::cerr << "Debug : Now, EventRti::setEventTree()\n";
#endif

	if (!checkEventMode(EventBase::ISS)) return;

	if (evTree == nullptr) setTree("EventRti", "Run time information");
	else tree = evTree;

	tree->Branch("rti", &fRti);
}

void EventRti::setEnvironment() {
#if Debug == true
	std::cerr << "Debug : Now, EventRti::setEnvironment()\n";
#endif
	if (!checkEventMode(EventBase::ISS)) return;

	AMSSetupR::RTI::UseLatest(6); // pass6
}

bool EventRti::processEvent(AMSEventR * event, AMSChain * chain) {
	if (!checkEventMode(EventBase::ISS)) return true;
	if (event == nullptr)	return false;

	AMSSetupR::RTI rti;
	event->GetRTI(rti);

	bool rebuildRTI = true;
	if (rti.utime == CurrUTime) rebuildRTI = false;
	else CurrUTime = rti.utime;
	
	fRti.uTime = event->UTime();
	
	MGClock::MTime * mtime = MGClock::ConvertFromUTimeToMTime(event->UTime(), MGClock::ClockType::UTC);
	fRti.dateUTC = (mtime->tm_year + 1900) * 10000 + (mtime->tm_mon+1) * 100 + (mtime->tm_mday);
	fRti.timeUTC = (mtime->tm_hour) * 10000 + (mtime->tm_min) * 100 + (mtime->tm_sec);

	// ISS information
	AMSSetupR * setup = AMSSetupR::gethead();
	double RPT[3] = {0};   // ISS coordinates (R, Phi, Theta) (GTOD)
	double VelPT[3] = {0}; // ISS velocity (Vel rad/sec, VelPhi rad, VelTheta rad)
	double YPR[3] = {0};   // ISS attitude (Yaw, Pitch, Roll)
	if (setup != 0) {
		float rpt[3], velpt[3], yaw = 0, pitch = 0, roll = 0;
		setup->getISSTLE(rpt, velpt, fRti.uTime);
		setup->getISSAtt(roll, pitch, yaw, fRti.uTime);
		RPT[0] = rpt[0]; RPT[1] = rpt[1]; RPT[2] = rpt[2];
		VelPT[0] = velpt[0]; VelPT[1] = velpt[1]; VelPT[2] = velpt[2];
		YPR[0] = yaw; YPR[1] = pitch; YPR[2] = roll;
	}
	for (int dir = 0; dir < 3; ++dir) {
		fRti.rptISS[dir] = RPT[dir];
		fRti.velISS[dir] = VelPT[dir];
		fRti.yprISS[dir] = YPR[dir];
	}

	// ISS solar array && backtracing (based on particle)
	int isInShadow = -1;
	if (event->NParticle() > 0) {
		AMSPoint ic;
		int idx = event->isInShadow(ic, 0);
		if      (idx == 0) isInShadow = 0;
		else if (idx == 1) isInShadow = 1;
	}
	fRti.isInShadow = isInShadow;

	if (!rebuildRTI) return selectEvent(event);

	initEvent();
	fStopwatch.start();

	fRti.isInShadow = isInShadow;

	fRti.flagRun = true;
	if (event->GetRTIStat() != 0) fRti.flagRun = false;
	if (event->IsBadRun("") != 0) fRti.flagRun = false;
	if (rti.good != 0) fRti.flagRun = false;

	// good second
	bool isGoodSecond = true;
	if ((rti.ntrig/rti.nev) < 0.98 ||
			rti.nerr < 0 || (rti.nerr/rti.nev) > 0.1 ||
			(rti.npart/rti.ntrig) < (0.07/1600*rti.ntrig) || (rti.npart/rti.ntrig) > 0.25 ||
			rti.npart <= 0 || rti.nev > 1800)
		isGoodSecond = false;
	else
		isGoodSecond = true;
	fRti.isGoodSecond = isGoodSecond;

	fRti.zenith = rti.zenith;
	for (int i = 0; i < 4; i++) {
		fRti.cfStormer[i] = (std::fabs(rti.cf[i][0]) > std::fabs(rti.cf[i][1])) ?
			std::fabs(rti.cf[i][0]) : std::fabs(rti.cf[i][1]);
		fRti.cfIGRF[i] = (std::fabs(rti.cfi[i][0]) > std::fabs(rti.cfi[i][1])) ?
			std::fabs(rti.cfi[i][0]) : std::fabs(rti.cfi[i][1]);
	}
	fRti.radiusGTOD = rti.r;
	fRti.thetaGTOD  = rti.theta;
	fRti.phiGTOD    = rti.phi;
	fRti.latGAT     = rti.glat * TMath::DegToRad();
	fRti.longGAT    = rti.glong * TMath::DegToRad();
	fRti.isInSAA    = rti.IsInSAA();
	fRti.uTime      = event->UTime();
	fRti.liveTime   = rti.lf * rti.nev / (rti.nev + rti.nerr);
	fRti.latMAG     = rti.getthetam();
	fRti.longMAG    = rti.getphim();
	
	for (int dir = 0; dir < 3; ++dir) {
		fRti.rptISS[dir] = RPT[dir];
		fRti.velISS[dir] = VelPT[dir];
		fRti.yprISS[dir] = YPR[dir];
	}

	// |PG-MD| < 35e-4 (L1), 45e-4 (L9) [cm]
	AMSPoint pn1, pn9, pd1, pd9;
	event->GetRTIdL1L9(0, pn1, pd1, event->UTime(), 60);
	event->GetRTIdL1L9(1, pn9, pd9, event->UTime(), 60);
	fRti.trackerAlign[0][0] = pd1.x();
	fRti.trackerAlign[0][1] = pd1.y();
	fRti.trackerAlign[1][0] = pd9.x();
	fRti.trackerAlign[1][1] = pd9.y();

	// Inner Tracker Temperature (Sensor A)
	std::vector<float> tempSensA;
	if (!setup->fSlowControl.GetData("Sensor A", event->Run(), 0, tempSensA) && 
	    tempSensA.size() > 0) {
		fRti.trackerTemp = tempSensA.at(0);
	}
	
	fStopwatch.stop();
	return selectEvent(event);
}

bool EventRti::selectEvent(AMSEventR * event) {
	// RTI cut
	if (!EventBase::checkEventMode(EventBase::ISS)) return true;

	if (!fRti.flagRun) return false;
	if (!fRti.isGoodSecond) return false;
	if (fRti.zenith > 40) return false;
	if (fRti.isInSAA) return false;
	if (fRti.liveTime < 0.5) return false;
	if (fRti.trackerAlign[0][1] > 35. || fRti.trackerAlign[1][1] > 45.) return false;

	if (fRti.isInShadow == 1) return false;

	return true;
}


//---- EventTrg ----//
EventTrg::EventTrg() : EventBase() {
}

EventTrg::~EventTrg() {
}

void EventTrg::initEvent() {
	fTrg.init();
}

void EventTrg::setEventTree(TTree * evTree) {
#if Debug == true
	std::cerr << "Debug : Now, EventTrg::setEventTree()\n";
#endif

	if (evTree == nullptr) setTree("EventTrg", "Anti-Coincidence Counter");
	else tree = evTree;

	tree->Branch("trg", &fTrg);
}

void EventTrg::setEnvironment() {
#if Debug == true
	std::cerr << "Debug : Now, EventTrg::setEnvironment()\n";
#endif
}

bool EventTrg::processEvent(AMSEventR * event, AMSChain * chain) {
	initEvent();
	if (event == nullptr)	return false;
	fStopwatch.start();

	Level1R * lvl1 = event->pLevel1(0);
	if (lvl1 == nullptr) return false;
	if (checkEventMode(EventBase::MC)) {
		// Rebuild according to Flight tr.setup
		lvl1->RebuildTrigPatt(fTrg.logicPatt, fTrg.physicalPatt);
	}
	else {
		lvl1->RestorePhysBPat();
		fTrg.physicalPatt = lvl1->PhysBPatt;
		fTrg.logicPatt = lvl1->JMembPatt;
	}

	// trigger info
	bool extTrg        = ((fTrg.physicalPatt&0x80) > 0);
	bool unBiasTrgTOF  = ((fTrg.physicalPatt&0x01) > 0);
	bool unBiasTrgECAL = ((fTrg.physicalPatt&0x40) > 0);
	bool physTrg       = ((fTrg.physicalPatt&0x3e) > 0);
	fTrg.bit = extTrg * 1 + unBiasTrgTOF * 2 + unBiasTrgECAL * 4 + physTrg * 8;

	fStopwatch.stop();
	return selectEvent(event);
}

bool EventTrg::selectEvent(AMSEventR * event) {
	if ((fTrg.bit&1) > 0) return false; // external

	return true;
}


//---- EventTof ----//
EventTof::EventTof() : EventBase() {
}

EventTof::~EventTof() {
}

void EventTof::initEvent() {
	fTof.init();
}

void EventTof::setEventTree(TTree * evTree) {
#if Debug == true
	std::cerr << "Debug : Now, EventTof::setEventTree()\n";
#endif

	if (evTree == nullptr) setTree("EventTof", "Time of Flight");
	else tree = evTree;

	tree->Branch("tof", &fTof);
}

void EventTof::setEnvironment() {
#if Debug == true
	std::cerr << "Debug : Now, EventTof::setEnvironment()\n";
#endif
}

bool EventTof::processEvent(AMSEventR * event, AMSChain * chain) {
	initEvent();
	if (event == nullptr)	return false;
	fStopwatch.start();

	fTof.numOfCls = event->NTofCluster();
	fTof.numOfClsH = event->NTofClusterH();
	fTof.numOfBeta = event->NBeta();
	fTof.numOfBetaH = event->NBetaH();
		
	while (recEv.iBeta >= 0) {
		BetaR * beta = event->pBeta(recEv.iBeta);
		if (beta == nullptr) break;
		fTof.statusBeta = true;
		fTof.beta = beta->Beta;
		break;
	}

	TofRecH::BuildOpt = 0; // normal

    const int qopt = TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor|TofRecH::kDAWeight|TofRecH::kQ2Q;
	const short pattIdx[4] = { 1, 2, 4, 8 };
	while (recEv.iBetaH >= 0 && event->pBetaH(recEv.iBetaH) != nullptr) {
		BetaHR* betaH = event->pBetaH(recEv.iBetaH);
        if (betaH->iTrTrack() != recEv.iTrTrack) break;

		int ncls[4] = {0};
		fTof.numOfInTimeCls = event->GetNTofClustersInTime(betaH, ncls);

		fTof.statusBetaH = true;
		fTof.betaH = betaH->GetBeta();
		fTof.betaHBit = ((betaH->pTrTrack   ()) ? 1 : 0) +
                        ((betaH->pTrdTrack  ()) ? 2 : 0) +
                        ((betaH->pEcalShower()) ? 4 : 0);
		fTof.normChisqT  = betaH->GetNormChi2T();
		fTof.normChisqC  = betaH->GetNormChi2C();

		fTof.betaHPatt = 0;
		for (int il = 0; il < 4; il++) {
			if (!betaH->TestExistHL(il)) continue;
			TofClusterHR* cls = betaH->GetClusterHL(il);
			if (cls == nullptr) continue;
			if (!cls->IsGoodTime()) continue;
            fTof.coo[il][0] = cls->Coo[0];
            fTof.coo[il][1] = cls->Coo[1];
            fTof.coo[il][2] = cls->Coo[2];
            fTof.err[il][0] = cls->ECoo[0];
            fTof.err[il][1] = cls->ECoo[1];
            fTof.err[il][2] = cls->ECoo[2];
			fTof.Q[il] = betaH->GetQL(il, 2, qopt);
			fTof.betaHPatt += pattIdx[il];
		}

		int nlay = 0;
		float Qall_RMS = 0;
		fTof.Qall = betaH->GetQ(nlay, Qall_RMS);
        
        float Zprob = 0;
        short Zall = betaH->GetZ(nlay, Zprob, 0);
        fTof.Zall = Zall;

		break;
	} // while loop - ibetaH > 0


	// Find Hits in the TOF supper layer (Time)
	std::vector<int> betaHClsId(4, -1);
	if (fTof.statusBetaH) {
		BetaHR * betaH = event->pBetaH(recEv.iBetaH);
		for (int it = 0; it < betaH->NTofClusterH(); ++it)
			betaHClsId.at(betaH->pTofClusterH(it)->Layer) = betaH->iTofClusterH(it);
	}
	
    const double ChrgLimit = 0.875;
	short nearHitNm[4] = { 0, 0, 0, 0 };
	for (int it = 0; it < event->NTofClusterH(); ++it) {
		TofClusterHR * cls = event->pTofClusterH(it);
		if (cls == nullptr) continue;
		if (betaHClsId.at(cls->Layer) == it) continue;
		int    lay  = cls->Layer;
		double chrg = cls->GetQSignal();
		if (chrg > ChrgLimit) nearHitNm[lay]++;
	}
	for (int il = 0; il < 4; ++il) {
		fTof.numOfExtCls[il] = nearHitNm[il];
	}

	fStopwatch.stop();
	return selectEvent(event);
}

bool EventTof::selectEvent(AMSEventR * event) {
	return true;
}


//---- EventAcc ----//
EventAcc::EventAcc() : EventBase() {
}

EventAcc::~EventAcc() {
}

void EventAcc::initEvent() {
	fAcc.init();
}

void EventAcc::setEventTree(TTree * evTree) {
#if Debug == true
	std::cerr << "Debug : Now, EventAcc::setEventTree()\n";
#endif

	if (evTree == nullptr) setTree("EventAcc", "Anti-Coincidence Counter");
	else tree = evTree;

	tree->Branch("acc", &fAcc);
}

void EventAcc::setEnvironment() {
#if Debug == true
	std::cerr << "Debug : Now, EventAcc::setEnvironment()\n";
#endif
}

bool EventAcc::processEvent(AMSEventR * event, AMSChain * chain) {
	initEvent();
	if (event == nullptr)	return false;
	fStopwatch.start();
	
	//event->RebuildAntiClusters();
	fAcc.numOfCls = event->NAntiCluster();

	const double TimeOfOneM = 3.335640e+00; // (speed of light)
	while (recEv.iBetaH >= 0 && event->pBetaH(recEv.iBetaH) != nullptr) {
		BetaHR * betaH = event->pBetaH(recEv.iBetaH);
		std::vector<float> time;
		for (int il = 0; il < 4; il++) {
			if (!betaH->TestExistHL(il)) continue;
			TofClusterHR * cls = betaH->GetClusterHL(il);
			if (cls == nullptr) continue;
			time.push_back(cls->Time);
		}
		if (time.size() == 0) break;
		std::sort(time.begin(), time.end());
		double timeRange[2] = { (time.front() - 5. * TimeOfOneM), (time.back()  + 10. * TimeOfOneM) };
		double minTimeOfTOF = time.front();

		for (int icls = 0; icls < event->NAntiCluster(); ++icls) {
			AntiClusterR * cls = event->pAntiCluster(icls);
			if (cls == nullptr) continue;
			if (cls->time < timeRange[0] || cls->time > timeRange[1]) continue;
			ClsACCInfo clsACC;
			clsACC.sector = cls->Sector;
			clsACC.time   = cls->time - minTimeOfTOF;
			clsACC.rawQ   = (MGNumc::EqualToZero(cls->rawq) ? -1 : cls->rawq);
			clsACC.coo[0] = cls->AntiCoo[0];
			clsACC.coo[1] = cls->AntiCoo[1];
			clsACC.coo[2] = cls->AntiCoo[2];
			fAcc.clusters.push_back(clsACC);
		}
		if (fAcc.clusters.size() >= 2)
			std::sort(fAcc.clusters.begin(), fAcc.clusters.end(), ClsACCInfo_sort());

		break;
	}


	fStopwatch.stop();
	return selectEvent(event);
}

bool EventAcc::selectEvent(AMSEventR * event) {
	return true;
}


//---- EventTrk ----//
EventTrk::EventTrk() : EventBase() {
}

EventTrk::~EventTrk() {
}

void EventTrk::initEvent() {
	fTrk.init();
}

void EventTrk::setEventTree(TTree * evTree) {
#if Debug == true
	std::cerr << "Debug : Now, EventTrk::setEventTree()\n";
#endif

	if (evTree == nullptr) setTree("EventTrk", "Silicon Tracker");
	else tree = evTree;

	tree->Branch("trk", &fTrk);
}

void EventTrk::setEnvironment() {
#if Debug == true
	std::cerr << "Debug : Now, EventTrk::setEnvironment()\n";
#endif

	// Disable overwriting of datacards from file
	TRMCFFKEY.ReadFromFile = 0;
	TRFITFFKEY.ReadFromFile = 0;

	// Enable latest alignment
	if (checkEventMode(EventBase::ISS)) {
		TkDBc::UseFinal();
	}
}

bool EventTrk::processEvent(AMSEventR * event, AMSChain * chain) {
	initEvent();
	if (event == nullptr)	return false;
	fStopwatch.start();

	// Beta Info
	float Beta = 0;
	if      (recEv.iBetaH >= 0) Beta = std::fabs(event->pBetaH(recEv.iBetaH)->GetBeta());
	else if (recEv.iBeta  >= 0) Beta = std::fabs(event->pBeta(recEv.iBeta)->Beta);
	else                        Beta = 1;
    
    fTrk.numOfTrack = event->NTrTrack();

    TrTrackR* trtk = (recEv.iTrTrack >= 0) ? event->pTrTrack(recEv.iTrTrack) : nullptr;
	short fitidInn = (trtk != nullptr) ? trtk->iTrTrackPar(1, 3, recEv.trRft(0, 0)) : -1;

	const unsigned short _hasL1  =   1;
	const unsigned short _hasL2  =   2;
	const unsigned short _hasL34 =  12;
	const unsigned short _hasL56 =  48;
	const unsigned short _hasL78 = 192;
	const unsigned short _hasL9  = 256;

    if (trtk != nullptr && fitidInn) {
		TrackInfo track;
		track.bitPattJ   = trtk->GetBitPatternJ();
		track.bitPattXYJ = trtk->GetBitPatternXYJ();
        
        short isInner   = ((track.bitPattJ&_hasL34) > 0 &&
		                   (track.bitPattJ&_hasL56) > 0 &&
						   (track.bitPattJ&_hasL78) > 0) ? 1 : 0;
		short isL2      = ((track.bitPattJ&_hasL2) > 0) ?  2 : 0;
		short isL1      = ((track.bitPattJ&_hasL1) > 0) ?  4 : 0;
		short isL9      = ((track.bitPattJ&_hasL9) > 0) ?  8 : 0;
		short bitPatt   = isInner + isL2 + isL1 + isL9;

        short isInnerXY = ((track.bitPattXYJ&_hasL34) > 0 &&
		                   (track.bitPattXYJ&_hasL56) > 0 &&
						   (track.bitPattXYJ&_hasL78) > 0) ? 1 : 0;
		short isL2XY    = ((track.bitPattXYJ&_hasL2) > 0) ?  2 : 0;
		short isL1XY    = ((track.bitPattXYJ&_hasL1) > 0) ?  4 : 0;
		short isL9XY    = ((track.bitPattXYJ&_hasL9) > 0) ?  8 : 0;
		short bitPattXY = isInnerXY + isL2XY + isL1XY + isL9XY;

		track.bitPatt   = bitPatt; 
		track.bitPattXY = bitPattXY; 
        track.QIn = trtk->GetInnerQ_all(Beta, fitidInn).Mean;
		track.QL2 = (isL2>0) ? trtk->GetLayerJQ(2, Beta) : -1;
		track.QL1 = (isL1>0) ? trtk->GetLayerJQ(1, Beta) : -1;
		track.QL9 = (isL9>0) ? trtk->GetLayerJQ(9, Beta) : -1;

		for (int ilay = 0; ilay < 9; ++ilay) {
			if (!trtk->TestHitLayerJ(ilay+1)) continue;
			TrRecHitR * recHit = trtk->GetHitLJ(ilay+1);
			if (recHit == nullptr) continue;

			int tkid = recHit->GetTkId();
			int mult = recHit->GetResolvedMultiplicity(); //  -1 resolved multiplicty coordinates
			                                              // > 0 requested multiplicty coordinates
            AMSPoint coo = (ilay==0 || ilay==8) ? (trtk->GetHitCooLJ(ilay+1, 0) + trtk->GetHitCooLJ(ilay+1, 1))*0.5 : trtk->GetHitCooLJ(ilay+1); // (CIEMAT+PG)/2
			
            TrClusterR* xcls = (recHit->GetXClusterIndex() >= 0 && recHit->GetXCluster()) ? recHit->GetXCluster() : nullptr;
			TrClusterR* ycls = (recHit->GetYClusterIndex() >= 0 && recHit->GetYCluster()) ? recHit->GetYCluster() : nullptr;
			
			TkSens tksens(coo, EventBase::checkEventMode(EventBase::MC));
			int sens = (tksens.LadFound()) ? tksens.GetSensor() : -1;

            double xloc = (xcls == nullptr || !tksens.LadFound() || MGNumc::Compare(std::fabs(tksens.GetImpactPointX()), 0.5) > 0) ? -1.0 : tksens.GetImpactPointX();
            double yloc = (ycls == nullptr || !tksens.LadFound() || MGNumc::Compare(std::fabs(tksens.GetImpactPointY()), 0.5) > 0) ? -1.0 : tksens.GetImpactPointY();

	        const int qopt = TrClusterR::kAsym | TrClusterR::kGain | TrClusterR::kLoss | TrClusterR::kMIP | TrClusterR::kAngle;
			float xadc = (xcls == nullptr || !TrCharge::GoodChargeReconHit(recHit, 0)) ? -1.0 : recHit->GetSignalCombination(0, qopt, 1, 0, 0); 
			float yadc = (ycls == nullptr || !TrCharge::GoodChargeReconHit(recHit, 1)) ? -1.0 : recHit->GetSignalCombination(1, qopt, 1, 0, 0); 


			std::vector<float> xstripSig;
			std::vector<float> xstripSgm;
			std::vector<float> xstripSS;
			for (int it = 0; (xcls!=nullptr) && (it < xcls->GetLength()); ++it) {
				xstripSig.push_back(xcls->GetSignal(it));
				xstripSgm.push_back(xcls->GetNoise(it));
                xstripSS.push_back(xcls->GetSignal(it)/xcls->GetNoise(it));
			}
			
			std::vector<float> ystripSig;
			std::vector<float> ystripSgm;
			std::vector<float> ystripSS;
			for (int it = 0; (ycls!=nullptr) && (it < ycls->GetLength()); ++it) {
				ystripSig.push_back(ycls->GetSignal(it));
				ystripSgm.push_back(ycls->GetNoise(it));
                ystripSS.push_back(ycls->GetSignal(it)/ycls->GetNoise(it));
			}
          
            const float GateTh = 0.3;
            const float LenTh  = 0.082;

            short xseedAddr = (xcls) ? xcls->GetSeedAddress() : -1;
            short xseedIndx = (xcls) ? xcls->GetSeedIndex() : -1;
            float xlenTh    = (xcls) ? LenTh*xstripSS.at(xseedIndx) : 0.;
            short xreg[2] = { xseedIndx, xseedIndx };
			for (int it = xseedIndx-1; (xcls!=nullptr) && it >= 0; --it)
                if (xstripSS.at(it) < xstripSS.at(it+1)+GateTh && xstripSS.at(it) > xlenTh) xreg[0] = it; else break;
			for (int it = xseedIndx+1; (xcls!=nullptr) && it < xstripSS.size(); ++it)
                if (xstripSS.at(it) < xstripSS.at(it-1)+GateTh && xstripSS.at(it) > xlenTh) xreg[1] = it; else break;
            
            short yseedAddr = (ycls) ? ycls->GetSeedAddress() : -1;
            short yseedIndx = (ycls) ? ycls->GetSeedIndex() : -1;
            float ylenTh    = (ycls) ? LenTh*ystripSS.at(yseedIndx) : 0.;
            short yreg[2] = { yseedIndx, yseedIndx };
			for (int it = yseedIndx-1; (ycls!=nullptr) && it >= 0; --it)
                if (ystripSS.at(it) < ystripSS.at(it+1)+GateTh && ystripSS.at(it) > ylenTh) yreg[0] = it; else break;
			for (int it = yseedIndx+1; (ycls!=nullptr) && it < ystripSS.size(); ++it)
                if (ystripSS.at(it) < ystripSS.at(it-1)+GateTh && ystripSS.at(it) > ylenTh) yreg[1] = it; else break;
          
			HitTRKInfo hit;
			hit.layJ    = ilay+1;
			hit.tkid    = tkid;
			hit.sens    = sens;
			hit.mult    = mult; 
			hit.side[0] = (xcls != nullptr);
			hit.side[1] = (ycls != nullptr);
			hit.coo[0]  = coo[0];
			hit.coo[1]  = coo[1];
			hit.coo[2]  = coo[2];
			hit.adc[0]  = xadc;
			hit.adc[1]  = yadc;
			hit.loc[0]  = xloc;
			hit.loc[1]  = yloc;
            hit.nsr[0]  = (xcls != nullptr) ? (xreg[1]-xreg[0]+1) : -1;
            hit.nsr[1]  = (ycls != nullptr) ? (yreg[1]-yreg[0]+1) : -1;
		
			track.hits.push_back(hit);
		} // for loop - layer
		if (track.hits.size() > 1) std::sort(track.hits.begin(), track.hits.end(), HitTRKInfo_sort());

		const short _nalgo = 2;
		const short _algo[_nalgo] = { 1, 6 };
		const short _npatt = 4;
		const short _patt[_npatt] = { 3, 5, 6, 7 };
        
        Int_t zin  = (track.QIn < 1.) ? 1 : std::lrint(track.QIn);
        float mass = (zin < 2) ? TrFit::Mproton : (0.5 * (TrFit::Mhelium) * zin);
		for (int algo = 0; algo < _nalgo; ++algo) {
			for (int patt = 0; patt < _npatt; ++patt) {
                MGClock::HrsStopwatch sw; sw.start();
                TrFit trFit;
				int fitid = trtk->iTrTrackPar(trFit, _algo[algo], _patt[patt], recEv.trRft(algo, patt), mass, zin);
                sw.stop();
				if (fitid < 0) continue;
				
                track.status[algo][patt] = true;
				track.rig[algo][patt] = trtk->GetRigidity(fitid, 0);
				track.chisq[algo][patt][0] = trtk->GetNormChisqX(fitid);
				track.chisq[algo][patt][1] = trtk->GetNormChisqY(fitid);
				track.chisq[algo][patt][2] = (trtk->GetChisqX(fitid) + trtk->GetChisqY(fitid)) / (trtk->GetNdofX(fitid) + trtk->GetNdofY(fitid));
			

                const int ustate = 0; // KALMAN
				for (int il = 0; il < 9; ++il) {
				    AMSPoint pntLJ;
				    AMSDir   dirLJ;
                    double   rig = track.rig[algo][patt];
                        
                    if (_algo[algo] == 6) // KALMAN
                        trFit.InterpolateKalman(recEv.trackerZJ[il], pntLJ, dirLJ, rig, ustate);
                    else
                        trtk->InterpolateLayerJ(il+1, pntLJ, dirLJ, fitid);
				   
                    if (MGNumc::EqualToZero(rig)) continue;
                    
                    track.stateLJ[algo][patt][il][0] = pntLJ[0];
				    track.stateLJ[algo][patt][il][1] = pntLJ[1];
				    track.stateLJ[algo][patt][il][2] = pntLJ[2];
				    track.stateLJ[algo][patt][il][3] = -dirLJ[0];
				    track.stateLJ[algo][patt][il][4] = -dirLJ[1];
				    track.stateLJ[algo][patt][il][5] = -dirLJ[2];
				    track.stateLJ[algo][patt][il][6] = rig;
                }

                track.cpuTime[algo][patt] = sw.time() * 1.0e3;
			} // for loop - pattern
		}

        // HChou Fitting
        TrackSys::TrFitPar fitPar(TrackSys::PartType::Proton);
        TrackSys::HitSt mhitL1;
        TrackSys::HitSt mhitL9;
        for (auto&& hit : track.hits) {
            TrackSys::HitSt mhit(hit.side[0], hit.side[1], hit.layJ);
            mhit.set_coo(hit.coo[0], hit.coo[1], hit.coo[2]);
            mhit.set_nsr(hit.nsr[0], hit.nsr[1]);
            mhit.set_adc(hit.adc[0], hit.adc[1]);
            if (hit.layJ >= 2 && hit.layJ <= 8) fitPar.addHit(mhit);
            else {
                if (hit.layJ == 1) mhitL1 = mhit;
                if (hit.layJ == 9) mhitL9 = mhit;
            }
        }

		for (int patt = 0; patt < _npatt; patt++) {
            const int algo = 2;
            TrackSys::TrFitPar _fitPar = fitPar;
            if (_patt[patt] == 5 || _patt[patt] == 7) { if (!mhitL1()) continue; _fitPar.addHit(mhitL1); }
            if (_patt[patt] == 6 || _patt[patt] == 7) { if (!mhitL9()) continue; _fitPar.addHit(mhitL9); }
           
            MGClock::HrsStopwatch sw; sw.start();
            //TrackSys::SimpleTrFit hctr(_fitPar);
            TrackSys::PhyTrFit    hctr(_fitPar);
            sw.stop();
            
            if (!hctr.status()) continue;

            track.status[algo][patt] = true;
			track.rig[algo][patt] = hctr.part().rig();
			track.chisq[algo][patt][0] = hctr.nchix();
			track.chisq[algo][patt][1] = hctr.nchiy();
			track.chisq[algo][patt][2] = hctr.nchi();

            for (Int_t il = 0; il < 9; ++il) {
                const TrackSys::PhySt* stt = hctr.stts(il+1);
                if (stt == nullptr) continue;
				track.stateLJ[algo][patt][il][0] = stt->cx();
				track.stateLJ[algo][patt][il][1] = stt->cy();
				track.stateLJ[algo][patt][il][2] = stt->cz();
				track.stateLJ[algo][patt][il][3] = stt->ux();
				track.stateLJ[algo][patt][il][4] = stt->uy();
				track.stateLJ[algo][patt][il][5] = stt->uz();
				track.stateLJ[algo][patt][il][6] = stt->rig();
            }



            //TrackSys::PhySt ppst(hctr.part());
            //for (int il = 0; il < 9; ++il) {
            //    TrackSys::PropMgnt::PropToZ(recEv.trackerZJ[il], ppst);
			//	track.stateLJ[algo][patt][il][0] = ppst.cx();
			//	track.stateLJ[algo][patt][il][1] = ppst.cy();
			//	track.stateLJ[algo][patt][il][2] = ppst.cz();
			//	track.stateLJ[algo][patt][il][3] = ppst.ux();
			//	track.stateLJ[algo][patt][il][4] = ppst.uy();
			//	track.stateLJ[algo][patt][il][5] = ppst.uz();
            //}
            
            track.cpuTime[algo][patt] = sw.time() * 1.0e3;
        }

		fTrk.track = track;
	}

    // Haino's tools
    if (trtk != nullptr && fitidInn) {
        fTrk.ftL56Dist     = std::min(event->GetTkFeetDist(5), event->GetTkFeetDist(6));
        fTrk.survHeL56Prob = event->GetHeSurvProbAtL56();

        float smin = 0;
        for (int il = 0; il < 7; il++) { // Loop on inner Layers 2-8
            // Get a ratio of raw ADC used for the hit over the sum of n=10 strips around.
            float sr = event->GetTrackerRawSignalRatio(il+2, 10);
            if (sr > 0 && (smin == 0 || sr < smin)) smin = sr;
            fTrk.ratN10S[il] = sr;
        }
        if (smin > 0) fTrk.ratN10Smin = smin;

        fTrk.noiseInTrSH = event->IsTrackPickingUpNoise();
        fTrk.betaSH[0]   = TrMass::GetBeta(event, 1, trtk); 
        fTrk.betaSH[1]   = TrMass::GetBeta(event, 11, trtk); 
        fTrk.betaSH[2]   = TrMass::GetBeta(event, 111, trtk); 
        fTrk.massEstSH   = TrMass::GetMQL(event, trtk);
    }

	fStopwatch.stop();
	return selectEvent(event);
}

bool EventTrk::selectEvent(AMSEventR * event) {
	return true;
}


//---- EventTrd ----//
EventTrd::EventTrd() : EventBase() {
}

EventTrd::~EventTrd() {
}

void EventTrd::initEvent() {
	fTrd.init();
}

void EventTrd::setEventTree(TTree * evTree) {
#if Debug == true
	std::cerr << "Debug : Now, EventTrd::setEventTree()\n";
#endif

	if (evTree == nullptr) setTree("EventTrd", "Transition Radiation");
	else tree = evTree;

	tree->Branch("trd", &fTrd);
}

void EventTrd::setEnvironment() {
#if Debug == true
	std::cerr << "Debug : Now, EventTrd::setEnvironment()\n";
#endif

	if (checkEventMode(EventBase::BT)) {
		TrdKCluster::IsReadGlobalAlignment = false;
	}
	if (checkEventMode(EventBase::MC)) {
		TrdKCluster::ForceReadAlignment = false;
		TrdKCluster::ForceReadCalibration = false;
	}
}

bool EventTrd::processEvent(AMSEventR * event, AMSChain * chain) {
	initEvent();
	if (event == nullptr) return false;
	fStopwatch.start();

	fTrd.numOfCls = event->NTrdCluster();
	fTrd.numOfSegment = event->NTrdSegment();
	fTrd.numOfTrack = event->NTrdTrack();
	fTrd.numOfHTrack = event->NTrdHTrack();
		
	// TrdKCluster
	float TOF_Beta = 1;
	if      (recEv.iBetaH >= 0) TOF_Beta = std::fabs(event->pBetaH(recEv.iBetaH)->GetBeta());
	else if (recEv.iBeta  >= 0) TOF_Beta = std::fabs(event->pBeta(recEv.iBeta)->Beta);
	
	TrdKCluster * trdkcls = TrdKCluster::gethead();
	for (int kindOfFit = 0; kindOfFit <= 1; ++kindOfFit) {
		if (!(checkEventMode(EventBase::BT) || (trdkcls->IsReadAlignmentOK == 2 && trdkcls->IsReadCalibOK == 1))) break;
		bool isOK = false;
		switch(kindOfFit) {
			case 0 :
				{
				  if      (recEv.iTrdHTrack < 0 && recEv.iTrdTrack  < 0) break;
				  else if (recEv.iTrdHTrack >= 0) {
				  	TrdHTrackR * trdh = event->pTrdHTrack(recEv.iTrdHTrack);
				  	trdkcls->Build(trdh);
				  }
				  else if (recEv.iTrdTrack  >= 0) {
				  	TrdTrackR * trd = event->pTrdTrack(recEv.iTrdTrack);
				  	trdkcls->Build(trd);
				  }
					else break;
				  isOK = true;
				}
				break;
			case 1 :
				{
				  if (recEv.iTrTrack < 0) break;
				  TrTrackR * trtk = event->pTrTrack(recEv.iTrTrack);
				  int fitid_max = trtk->iTrTrackPar(1, 0, 22);
				  if (fitid_max < 0) break;
				  trdkcls->SetTrTrack(trtk, fitid_max);
				  isOK = true;
				}
				break;
			default :
				break;
		}
		if (!isOK) continue;

		// It speeds lots of time 0.02 sec
		float Q = -1;
		//float Qerror = -1;
		//int   QnumberOfHit = -1;
		int   Qstatus = trdkcls->CalculateTRDCharge(0, TOF_Beta);
		if (Qstatus >= 0) {
			Q = trdkcls->GetTRDCharge();
			//Qerror = trdkcls->GetTRDChargeError();
			//QnumberOfHit = trdkcls->GetQNHit();
		}
		else continue;

		int nhits = 0; //To be filled with number of hits taken into account in Likelihood Calculation
		float threshold = 15; //ADC above which will be taken into account in Likelihood Calculation,  15 ADC is the recommended value for the moment.
		double llr[3] = {-1, -1, -1}; //To be filled with 3 LikelihoodRatio :  e/P, e/H, P/H
		if      (kindOfFit == 0) trdkcls->GetLikelihoodRatio_TRDRefit(threshold, llr, nhits);
		else if (kindOfFit == 1) trdkcls->GetLikelihoodRatio_TrTrack(threshold, llr, nhits);
		if (llr[0] < 0 || llr[1] < 0 || llr[2] < 0) continue;

		int numberOfHit = 0;
		for (int ih = 0; ih < nhits; ih++) {
			TrdKHit * hit = trdkcls->GetHit(ih);
			if (hit == nullptr) continue;
			if (checkEventMode(EventBase::ISS) || checkEventMode(EventBase::BT))
				if (!hit->IsCalibrated) continue;
			if (checkEventMode(EventBase::ISS))
				if (!hit->IsAligned) continue;
			//int lay = hit->TRDHit_Layer;
			//float amp = hit->TRDHit_Amp;
			numberOfHit++;
		}
		if (numberOfHit <= 0) continue;

		fTrd.statusKCls[kindOfFit] = true;
		fTrd.LLRep[kindOfFit]      = llr[0];
		fTrd.LLReh[kindOfFit]      = llr[1];
		fTrd.LLRph[kindOfFit]      = llr[2];
		fTrd.LLRnhit[kindOfFit]    = numberOfHit;
		fTrd.Q[kindOfFit]          = Q;
	}

	short trdIdx = -1;
	if      (recEv.iTrdHTrack >= 0) trdIdx = 1;
	else if (recEv.iTrdTrack  >= 0) trdIdx = 0;

	while (trdIdx >= 0) {
		bool isOK = false;
		AMSPoint trd_coo;
		AMSDir trd_dir;
		// Base on TrdTrack
		if (trdIdx == 0) {
			TrdTrackR * trd = event->pTrdTrack(recEv.iTrdTrack);
			trd_coo.setp(trd->Coo[0], trd->Coo[1], trd->Coo[2]);
			trd_dir.SetTheta(trd->Theta);
			trd_dir.SetPhi(trd->Phi);
			trd_dir[0] *= -1; trd_dir[1] *= -1; trd_dir[2] *= -1;
			isOK = true;
		}
		// Base on TrdHTrack
		if (trdIdx == 1) {
			TrdHTrackR * trdh = event->pTrdHTrack(recEv.iTrdHTrack);
			trd_coo.setp(trdh->Coo[0], trdh->Coo[1], trdh->Coo[2]);
			trd_dir.setd(trdh->Dir[0], trdh->Dir[1], trdh->Dir[2]);
			trd_dir[0] *= -1; trd_dir[1] *= -1; trd_dir[2] *= -1;
			isOK = true;
		}
		if (!isOK) break;
		else isOK = true;

		fTrd.trackStatus = true;
		fTrd.trackState[0] = trd_coo[0];
		fTrd.trackState[1] = trd_coo[1];
		fTrd.trackState[2] = trd_coo[2];
		fTrd.trackState[3] = trd_dir[0];
		fTrd.trackState[4] = trd_dir[1];
		fTrd.trackState[5] = trd_dir[2];

		break;
	} // while loop --- trd track

    TRDVertex trdVtx;
    trdVtx.Reconstruction(event);

    fTrd.vtxNum[0] = trdVtx.nvertex_3d;
    fTrd.vtxNum[1] = trdVtx.nvertex_2d_y;
    fTrd.vtxNum[2] = trdVtx.nvertex_2d_x;
    fTrd.vtxNTrk   = trdVtx.vertex_ntrack;
    fTrd.vtxNHit   = trdVtx.vertex_nhit;
    fTrd.vtxChi2   = trdVtx.vertex_chi2;
    fTrd.vtxCoo[0] = trdVtx.vertex_x;
    fTrd.vtxCoo[1] = trdVtx.vertex_y;
    fTrd.vtxCoo[2] = trdVtx.vertex_z;

	fStopwatch.stop();
	return selectEvent(event);
}

bool EventTrd::selectEvent(AMSEventR * event) {
	return true;
}


//---- EventRich ----//
EventRich::EventRich() : EventBase() {
}

EventRich::~EventRich() {
}

void EventRich::initEvent() {
	fRich.init();
}

void EventRich::setEventTree(TTree * evTree) {
#if Debug == true
	std::cerr << "Debug : Now, EventRich::setEventTree()\n";
#endif

	if (evTree == nullptr) setTree("EventRich", "Ring Imaging Cherenkov");
	else tree = evTree;

	tree->Branch("rich", &fRich);
}

void EventRich::setEnvironment() {
#if Debug == true
	std::cerr << "Debug : Now, EventRich::setEnvironment()\n";
#endif

	if (checkEventMode(EventBase::ISS) || checkEventMode(EventBase::MC)) {
		RichRingR::setBetaCorrection(RichRingR::fullUniformityCorrection);
	}
	if (checkEventMode(EventBase::BT)) {
		RichRingR::setBetaCorrection(RichRingR::noCorrection);
	}
		
	TString RichDefaultAGLTables = Form("%s/v5.00/RichDefaultAGLTables.04.dat",getenv("AMSDataDir"));
	RichOffline::RichRadiatorTileManager::Init((char*)RichDefaultAGLTables.Data());
}

bool EventRich::processEvent(AMSEventR * event, AMSChain * chain) {
	initEvent();
	if (event == nullptr)	return false;
	fStopwatch.start();

	// official RichRingR - start
    fRich.numOfHit = event->NRichHit();
	while (recEv.iRichRing >= 0) {
		RichRingR* rich = event->pRichRing(recEv.iRichRing);
        if (rich == nullptr) break;
		int kindOfRad = rich->IsNaF() ? 1 : 0;
		int tileOfRad = rich->getTileIndex();

		fRich.status = true;
		fRich.beta = rich->getBeta();
        fRich.kind = kindOfRad;
        fRich.tile = tileOfRad;
		fRich.Q = rich->getCharge2Estimate(true);
		fRich.Q = (fRich.Q > 1.0e-3) ? std::sqrt(fRich.Q) : -1;
	
	    // Rich cuts from Javie/Jorge
        const float cut_prob = 0.01;     // Kolmogorov test prob
        const float cut_pmt = 3;         // number of PMTs
	    const float cut_chrgConsistency = 5; // hit/PMT charge consistency test
	    const float cut_betaConsistency[2] = {0.01, 0.005};  // beta_lip vs beta_ciemat consistency (NaF, Aerogel)
	    const float cut_collPhe[2] = {0.6, 0.4};             // ring phe / total phe (NaF, Aerogel)
	    const float cut_expPhe[2] = {1, 2};                  // expected number of phe (NaF, Aerogel)

		fRich.isGood =
            (rich->IsGood() &&
             rich->getPMTs() >= cut_pmt &&
             rich->getProb() > cut_prob &&
             rich->getPMTChargeConsistency() < cut_chrgConsistency &&
             rich->getBetaConsistency() < cut_betaConsistency[kindOfRad]);

        fRich.numOfExpPE = rich->getExpectedPhotoelectrons();
        fRich.eftOfColPE = rich->getPhotoElectrons()/RichHitR::getCollectedPhotoElectrons();

		break;
	}
	// official RichRingR - end

    
    // RichVeto - start
	const float richPMTZ = -121.89;                      // pmt z-axis
	const float richRadZ[2] = {-74.35, -75.5 };             // aerogel / NaF
	const float cut_aerogelExternalBorder = 3350;           // aerogel external border (r**2)
	const float cut_aerogelNafBorder[2] = {17.475, 17.525}; // aerogel/NaF border (NaF, Aerogel)
	
    const int nBadTile = 5;
	const int kBadTile_Offical[nBadTile]  = { 3, 7, 87, 100, 108 }; // tiles with bad beta reconstruction
	const int kBadTile_MgntTile[nBadTile] = { 13, 23, 58, 86, 91 }; // tiles with bad beta reconstruction

    AMSDir   oth; // emission
    AMSPoint ems; // emission
    AMSPoint rev; // receiving
	double trRigAbs = 0;
    double trTheta[3] = {0};
	while (recEv.iTrTrack >= 0) {
		TrTrackR * trtk = event->pTrTrack(recEv.iTrTrack);
		if (trtk == nullptr) break;
		int fitid = trtk->iTrTrackPar(1, 3, recEv.trRft(0, 0));
		if (fitid < 0) break;

		AMSPoint ems_coo;
		AMSDir   ems_dir;
		short    tmp_kind = -1;
		short    tmp_tile = -1;
		trtk->Interpolate(richRadZ[0], ems_coo, ems_dir, fitid);
		for (Int_t it = 0; it < 3; ++it) {
			RichOffline::TrTrack tmp_richTrack(ems_coo, ems_dir);
			RichOffline::RichRadiatorTileManager tmp_mgntTile(&tmp_richTrack);
			trtk->Interpolate(tmp_mgntTile.getemissionpoint()[2], ems_coo, ems_dir, fitid);
			tmp_kind = tmp_mgntTile.getkind();
			tmp_tile = tmp_mgntTile.getcurrenttile();
		}
		
        // Radiator tile manager
		RichOffline::TrTrack richTrack(ems_coo, ems_dir);
		RichOffline::RichRadiatorTileManager mgntTile(&richTrack);
		if (mgntTile.getkind() < 1 || mgntTile.getkind() > 2) break;
		if (tmp_kind != mgntTile.getkind()) break;
		if (tmp_tile != mgntTile.getcurrenttile()) break;
		
        AMSPoint rev_coo;
		AMSDir   rev_dir;
		trtk->Interpolate(richPMTZ, rev_coo, rev_dir, fitid);
        
        oth = AMSDir(-ems_dir[0], -ems_dir[1], -ems_dir[2]);
        ems = ems_coo;
        rev = rev_coo;
        
        fRich.vetoKind         = mgntTile.getkind() - 1;
		fRich.vetoTile         = mgntTile.getcurrenttile();
		fRich.vetoRfrIndex     = mgntTile.getindex();
		fRich.vetoDistToBorder = mgntTile.getdistance();
		
        // geometry cut
		bool isStruct = false;
		AMSPoint richems = mgntTile.getemissionpoint();
		if (fRich.vetoKind == 0) {
			if ((richems[0]*richems[0]+richems[1]*richems[1]) > cut_aerogelExternalBorder)        isStruct = true;
			if (std::max(std::fabs(richems[0]), std::fabs(richems[1])) < cut_aerogelNafBorder[1]) isStruct = true;
		}
        else if (fRich.vetoKind == 1 && std::max(std::fabs(richems[0]), std::fabs(richems[1])) > cut_aerogelNafBorder[0]) isStruct = true;
		
        fRich.vetoIsInFiducialVolume = !isStruct;

		trRigAbs = std::fabs(trtk->GetRigidity(fitid, 0));
		
        const int    npart = 3;
		const double masschrg[npart] = 
		  { 0.000510999,   // electron
		    0.938272297,   // proton
		    1.876123915 }; // deuterium

		float numOfExpPE[npart] = { 0, 0, 0 };
		for (int it = 0; it < npart; ++it) {
		    double beta = 1.0 / std::hypot(1.0, (masschrg[it] / trRigAbs));
            double theta = std::acos(1.0/fRich.vetoRfrIndex/beta);
		    double exppe = RichRingR::ComputeNpExp(trtk, beta, 1.0);
		    numOfExpPE[it] = exppe;
            trTheta[it] = (MGNumc::Valid(theta) ? theta : -1.0);
		}
		std::copy(numOfExpPE, numOfExpPE+npart, fRich.vetoNumOfExpPE);

        break;
    }	

    //---- RICH HIT ----//
    //------------------------------------------------------------//
	// Calculate reflective point
	// Iterator Mothod :
	// emission point : e{x, y, z}
	// hit      point : h{x, y, z}
	// bisector point : b{x, y, z}
	// bisector coord : (parameter 0 <= bt <= 1)
	//     b{x, y, z} = (1 - bt) * e{x, y, z} + (bt) * h{x, y, z}
	//     br = sqrt(bx^2 + by^2)
	//
	// =============mirror=============
	//                /|\
	//               / | \
	//              /  |  \
	//             /   |   \
	//            /    |    \
	//           /     |     \
	//          /      |      \
	//         /       |       \
	//        /        |        \
	//       /         |         \
	//     "e"---(t)--"b"-(1-t)--"h"
	//
	// Angle bisector theorem :
	// mirror eq : (mz*z + mr*r + m0 = 0)
	//     mz = -1.489637e-01
	//     mr = -1.00
	//     m0 =  4.891730e+01
	// reflective point :
	//     fz = ( mr*(mr*bz - mz*br) - mz*m0) / (mz*mz + mr*mr)
	//     fr = (-mz*(mr*bz - mz*br) - mr*m0) / (mz*mz + mr*mr)
	//     fx = fr * (bx / sqrt(bx^2 + by^2))
	//     fy = fr * (by / sqrt(bx^2 + by^2))
	// length of emission to reflective : elen
	//     elen = sqrt((fx-ex)^2 + (fy-ey)^2 + (fz-ez)^2)
	// length of hit      to reflective : hlen
	//     hlen = sqrt((fx-hx)^2 + (fy-hy)^2 + (fz-hz)^2)
	// bisector parameter : bt (initial value 0.5)
	//     bt = elen / (elen + hlen)
    //------------------------------------------------------------//
    
    // light guide : channel id
    //  12 | 13 | 14 | 15
    //  -----------------
    //   8 |  9 | 10 | 11
    //  -----------------
    //   4 |  5 |  6 |  7
    //  -----------------
    //   0 |  1 |  2 |  3
    const float lg_rfrIndex      = 1.49; // light guide refractive index
    const float lg_height        = 3.00;
    const float lg_top_length    = 3.40;
    const float lg_bottom_length = 1.77;

    for (int it = 0; fRich.vetoKind >= 0 && it < event->NRichHit(); ++it) {
		RichHitR* hit = event->pRichHit(it);
		if (hit == nullptr) continue;
        double dist = rev.dist(hit->Coo);
		
        // dtha
        double dtha = -1.;
        double dphi = -1.;
		{
			AMSDir vacph(AMSPoint(hit->Coo) - ems);
            double rdtha = (std::sin(TMath::Pi() - vacph.gettheta()) / fRich.vetoRfrIndex);
            if (rdtha < 1.0) {
			    double theta = TMath::Pi() - std::asin(rdtha);
			    double phi   = vacph.getphi();
			    AMSDir emsph(theta, phi);
			    dtha = 2.0*std::asin(0.5*oth.dist(emsph));
                dphi = std::asin(std::sin(TMath::Pi() - vacph.gettheta()) / lg_rfrIndex);
            }
		}
        // rtha
        double rtha = -1.;
        double rphi = -1.;
		{
			const double mParZ = -1.489637e-01;
			const double mParR = -1.00;
			const double mPar0 = 4.891730e+01;
			double epnt[3] = { ems[0], ems[1], ems[2] };
			double hpnt[3] = { hit->Coo[0], hit->Coo[1], hit->Coo[2] };
			double initbt = 0.5;
			{
				double ez = epnt[2];
				double er = std::sqrt(epnt[0]*epnt[0] + epnt[1]*epnt[1]);
				double distbe = std::fabs(mParZ * ez + mParR * er + mPar0) / std::sqrt(ez*ez + er*er); 
				double hz = hpnt[2];
				double hr = std::sqrt(hpnt[0]*hpnt[0] + hpnt[1]*hpnt[1]);
				double distbh = std::fabs(mParZ * hz + mParR * hr + mPar0) / std::sqrt(hz*hz + hr*hr);
				initbt = distbe/(distbe+distbh);
			}
			double rflz = 0;
			double bt = initbt;
			double fineph[3] = { 0, 0, 0 };
            double pmtph[3] = { 0, 0, 0 };

			const int MaxIter = 30;
			const double ToleranceLimit = 0.005;
			int iter = 1;
			double tolerance = 1.0;
			while (iter <= MaxIter && tolerance > ToleranceLimit) {
				double bpnt[3] = {
					(1 - bt) * epnt[0] + bt * hpnt[0],
					(1 - bt) * epnt[1] + bt * hpnt[1],
					(1 - bt) * epnt[2] + bt * hpnt[2],
				};
				double bz = bpnt[2];
				double br = std::sqrt(bpnt[0]*bpnt[0] + bpnt[1]*bpnt[1]);
				if (MGNumc::EqualToZero(br)) break;

				double fz = ( mParR*(mParR*bz - mParZ*br) - mParZ*mPar0) / (mParZ*mParZ + mParR*mParR);
				double fr = (-mParZ*(mParR*bz - mParZ*br) - mParR*mPar0) / (mParZ*mParZ + mParR*mParR);
				double fpnt[3] = {
					fr * bpnt[0] / br,
					fr * bpnt[1] / br,
					fz
				};

				double edlen[3] = { (fpnt[0]-epnt[0]), (fpnt[1]-epnt[1]), (fpnt[2]-epnt[2]) };
				double elen = std::sqrt(edlen[0]*edlen[0] + edlen[1]*edlen[1] + edlen[2]*edlen[2]);
				
				double hdlen[3] = { (fpnt[0]-hpnt[0]), (fpnt[1]-hpnt[1]), (fpnt[2]-hpnt[2]) };
				double hlen = std::sqrt(hdlen[0]*hdlen[0] + hdlen[1]*hdlen[1] + hdlen[2]*hdlen[2]);

				double predbt = elen / (elen + hlen);
				tolerance = std::fabs(predbt - bt);
				iter++;
				
				if (tolerance < ToleranceLimit) {
					rflz = fz;
					fineph[0] = (fpnt[0] - epnt[0]);
					fineph[1] = (fpnt[1] - epnt[1]);
					fineph[2] = (fpnt[2] - epnt[2]);
                    pmtph[0]  = (hpnt[0] - fpnt[0]);
                    pmtph[1]  = (hpnt[1] - fpnt[1]);
                    pmtph[2]  = (hpnt[2] - fpnt[2]);
				}
				else bt = predbt;
			}
			
            bool success = (tolerance < ToleranceLimit && rflz < epnt[2] && rflz > hpnt[2]);
			if (success) {
				AMSDir vacph(fineph[0], fineph[1], fineph[2]);
                double rdtha = (std::sin(TMath::Pi() - vacph.gettheta()) / fRich.vetoRfrIndex);
                if (rdtha < 1.0) {
				    double theta = TMath::Pi() - std::asin(rdtha);
				    double phi   = vacph.getphi();
				    AMSDir emsph(theta, phi);
				    rtha = 2.0*std::asin(0.5*oth.dist(emsph));

                    AMSDir revph(pmtph[0], pmtph[1], pmtph[2]);
                    rphi = std::asin(std::sin(TMath::Pi() - revph.gettheta()) / lg_rfrIndex);
                }
                else success = false;
			}
		} // rtha
        
        HitRICHInfo hinfo;
        hinfo.status  = hit->Status;
        hinfo.cross   = hit->IsCrossed();
		hinfo.channel = hit->Channel;
        hinfo.npe     = hit->Npe;
		hinfo.coo[0]  = hit->Coo[0];
		hinfo.coo[1]  = hit->Coo[1];
		hinfo.coo[2]  = hit->Coo[2];
        hinfo.dist    = dist;
        hinfo.dtha    = dtha;
        hinfo.dphi    = dphi;
        hinfo.rtha    = rtha;
        hinfo.rphi    = rphi;

        fRich.hits.push_back(hinfo);
    } // hit - for loop
    if (fRich.hits.size() > 2) std::sort(fRich.hits.begin(), fRich.hits.end(), HitRICHInfo_sort());
   

    const float noise_npe = 0.4; // noise npe
    if (fRich.hits.size() != 0) {
		const double thetaLimit   = std::acos(1./fRich.vetoRfrIndex) * 1.05;
		double       linedist     = ems.dist(rev);
		double       mscatDirFluc = 0.0136 / ((trRigAbs>0.3)?trRigAbs:0.3);
		double       mscatCooFluc = mscatDirFluc * linedist;
		const double measDirFluc  = 0.01;
		const double measCooFluc  = 1.0;
		double       dirSgm       = 3.5 * std::sqrt(mscatDirFluc * mscatDirFluc + measDirFluc * measDirFluc);
		double       cooSgm       = 5.0 * std::sqrt(mscatCooFluc * mscatCooFluc + measCooFluc * measCooFluc);
   
		const int npart = 3;
		int cntCrs = 0;;
		int cntCkv[npart][3];
        std::fill_n(cntCkv[0], npart*3, 0);
        for (auto&& hit : fRich.hits) {
            if (hit.npe < noise_npe) continue;
            if (hit.dist < cooSgm) cntCrs++;
            else {
			    bool isInsideDIR = (hit.dtha > 0.0 && hit.dtha < thetaLimit);
			    bool isInsideRFL = (hit.rtha > 0.0 && hit.rtha < thetaLimit);
			    bool nonPhys = (!isInsideDIR && !isInsideRFL);

			    for (int it = 0; it < npart; ++it) {
                    if (trTheta[it] < 0.0) continue;
			    	bool isThetaDIR = (std::fabs(hit.dtha-trTheta[it]) < dirSgm);
			    	bool isThetaRFL = (std::fabs(hit.rtha-trTheta[it]) < dirSgm);
			    	bool isTheta = (isThetaDIR || isThetaRFL);
			    	if      (isTheta) cntCkv[it][0]++;
			    	else if (nonPhys) cntCkv[it][2]++;
			    	else              cntCkv[it][1]++;
			    }
            }
        }

        fRich.vetoNumOfCrsHit = cntCrs;
		for (int it = 0; it < npart; ++it)
            fRich.vetoNumOfCkvHit[it][0] = cntCkv[it][0];
    }
	// RichVeto - end

	fStopwatch.stop();
	return selectEvent(event);
}

bool EventRich::selectEvent(AMSEventR * event) {
	return true;
}

//---- EventEcal ----//
EventEcal::EventEcal() : EventBase() {
}

EventEcal::~EventEcal() {
}

void EventEcal::initEvent() {
	fEcal.init();
}

void EventEcal::setEventTree(TTree * evTree) {
#if Debug == true
	std::cerr << "Debug : Now, EventEcal::setEventTree()\n";
#endif

	if (evTree == nullptr) setTree("EventEcal", "Electromagnetic Calorimeter");
	else tree = evTree;

	tree->Branch("ecal", &fEcal);
}

void EventEcal::setEnvironment() {
#if Debug == true
	std::cerr << "Debug : Now, EventEcal::setEnvironment()\n";
#endif

	EcalShowerR::enableAutomaticRecoveryOfBrokenBuilds = false;
	EcalHadron::InitParameters();
}

bool EventEcal::processEvent(AMSEventR * event, AMSChain * chain) {
	initEvent();
	if (event == nullptr)	return false;
	fStopwatch.start();

	fEcal.numOfShower = event->NEcalShower();

    EcalShowerR* ecal = ((recEv.iEcalShower >= 0) ? event->pEcalShower(recEv.iEcalShower) : nullptr);
	if (ecal != nullptr) {
		ShowerInfo shower;
	
		shower.energyD = 1.e-3 * ecal->EnergyD;
		shower.energyE = ecal->EnergyE;
		shower.PisaBDT = ecal->GetEcalBDT();
		
		// Charge estimator based on ECAl-only;
		// it is advised to discard low-rigidity events
		// and to use events having only one EcalShower.
		shower.Q = ecal->EcalChargeEstimator();
		if (shower.Q < 1e-3) shower.Q = -1;

		// hadron shower
		int zeh = 1;
		if (recEv.iTrTrack >= 0) {
			TrTrackR * trtk = event->pTrTrack(recEv.iTrTrack);
			std::vector<like_t> probLvl;
			trtk->GetInnerZ(probLvl);
            if (probLvl.size() != 0)
    	    zeh = probLvl.at(0).Z;
		}
		else if (recEv.iBetaH >= 0 && event->pBetaH(recEv.iBetaH) != 0) {
			BetaHR * betaH = event->pBetaH(recEv.iBetaH);
			int nlay = 0; float prob = 0;
			TofChargeHR tofQH = betaH->gTofCharge();
			zeh = tofQH.GetZ(nlay, prob, 0);
		}

		EcalHadron::Build(ecal, zeh);
		shower.hadronApex = EcalHadron::EcalApex;
		shower.hadronEnergy = EcalHadron::EcalRigidity;

		fEcal.shower = shower;
	}

	fStopwatch.stop();
	return selectEvent(event);
}

bool EventEcal::selectEvent(AMSEventR * event) {
	return true;
}


//---- DataSelection ----//
DataSelection::SWITCH DataSelection::option[DataSelection::NUMBER] = {
	DataSelection::ON
};

DataSelection::DataSelection() {
#if Debug == true
	std::cerr << "Debug : Now, DataSelection::DataSelection()\n";
#endif

	isMultiTree = false;
	evTree = 0;
}

DataSelection::~DataSelection() {
#if Debug == true
	std::cerr << "Debug : Now, DataSelection::~DataSelection()\n";
#endif

	if (evTree != 0) delete evTree;
	evTree = 0;
}

void DataSelection::setMultiTree(bool isMTree) {
#if Debug == true
	std::cerr << "Debug : Now, DataSelection::setMultiTree()\n";
#endif

	isMultiTree = isMTree;
}

void DataSelection::setEventTree() {
#if Debug == true
	std::cerr << "Debug : Now, DataSelection::setEventTree()\n";
#endif

	if (isMultiTree == false) evTree = new TTree("data", "AMS data");
	else evTree = 0;

	if (checkOption(DataSelection::LIST)) list.setEventTree(evTree);
	if (checkOption(DataSelection::RTI)) rti.setEventTree(evTree);
	if (checkOption(DataSelection::TRG)) trg.setEventTree(evTree);
	if (checkOption(DataSelection::TOF)) tof.setEventTree(evTree);
	if (checkOption(DataSelection::ACC)) acc.setEventTree(evTree);
	if (checkOption(DataSelection::TRK)) trk.setEventTree(evTree);
	if (checkOption(DataSelection::TRD)) trd.setEventTree(evTree);
	if (checkOption(DataSelection::RICH)) rich.setEventTree(evTree);
	if (checkOption(DataSelection::ECAL)) ecal.setEventTree(evTree);
}

void DataSelection::setEnvironment() {
#if Debug == true
	std::cerr << "Debug : Now, DataSelection::setEnvironment()\n";
#endif

	list.setEnvironment();
	rti.setEnvironment();
	trg.setEnvironment();
	tof.setEnvironment();
	acc.setEnvironment();
	trk.setEnvironment();
	trd.setEnvironment();
	rich.setEnvironment();
	ecal.setEnvironment();

  // set scale function
  DataSelection::gScaleFact = 0.02;
  DataSelection::gScaleFunc1D.SetParameter(0, DataSelection::gScaleFact);
  DataSelection::gScaleFunc2D.SetParameter(0, DataSelection::gScaleFact);
}

int DataSelection::processEvent(AMSEventR * event, AMSChain * chain) {
#if Debug == true
	//std::cerr << "Debug : Now, DataSelection::processEvent()\n";
#endif

	if (event == nullptr) return -1;

	bool statusList = true;
	//bool statusRti = true;
	bool statusTrg = true;
	bool statusTof = true;
	bool statusAcc = true;
	bool statusTrk = true;
	bool statusTrd = true;
	bool statusRich = true;
	bool statusEcal = true;

	MGClock::HrsStopwatch fStopwatch;
	fStopwatch.start();

	if (checkOption(DataSelection::LIST)) statusList = list.processEvent(event, chain);
	if (!statusList) return -101;

	//if (checkOption(DataSelection::RTI)) statusRti = rti.processEvent(event);
	//if (!statusRti) return -102;

	if (checkOption(DataSelection::TRG)) statusTrg = trg.processEvent(event);
	if (!statusTrg) return -103;

	if (checkOption(DataSelection::ACC)) statusAcc = acc.processEvent(event);
	if (!statusAcc) return -105;

	if (checkOption(DataSelection::TRK)) statusTrk = trk.processEvent(event);
	if (!statusTrk) return -106;

	if (checkOption(DataSelection::TRD)) statusTrd = trd.processEvent(event);
	if (!statusTrd) return -107;

	if (checkOption(DataSelection::RICH)) statusRich = rich.processEvent(event);
	if (!statusRich) return -108;

	if (checkOption(DataSelection::ECAL)) statusEcal = ecal.processEvent(event);
	if (!statusEcal) return -109;

	// due to rebuild BetaH (put on last one)
	if (checkOption(DataSelection::TOF)) statusTof = tof.processEvent(event);
	if (!statusTof) return -104;

	fStopwatch.stop();

#if Debug == true
	const float limitfStopwatch = 5.0;
	float totlTime = fStopwatch.time() + recEv.time() + rti.time();
	if (totlTime > limitfStopwatch) {
		COUT("\nRUN %u  EVENT %u\n", event->Run(), event->Event());
		COUT("REAL TIME : %14.8f (SEC)   100.00%\n", totlTime);
		COUT("    RECON   %14.8f (SEC)   %6.2f%\n", recEv.time(), recEv.time() / totlTime * 100);
		COUT("     LIST   %14.8f (SEC)   %6.2f%\n", list.time(),  list.time() / totlTime * 100);
		COUT("      RTI   %14.8f (SEC)   %6.2f%\n", rti.time(),   rti.time() / totlTime * 100);
		COUT("      TRG   %14.8f (SEC)   %6.2f%\n", trg.time(),   trg.time() / totlTime * 100);
		COUT("      TOF   %14.8f (SEC)   %6.2f%\n", tof.time(),   tof.time() / totlTime * 100);
		COUT("      ACC   %14.8f (SEC)   %6.2f%\n", acc.time(),   acc.time() / totlTime * 100);
		COUT("      TRK   %14.8f (SEC)   %6.2f%\n", trk.time(),   trk.time() / totlTime * 100);
		COUT("      TRD   %14.8f (SEC)   %6.2f%\n", trd.time(),   trd.time() / totlTime * 100);
		COUT("     RICH   %14.8f (SEC)   %6.2f%\n", rich.time(),  rich.time() / totlTime * 100);
		COUT("     ECAL   %14.8f (SEC)   %6.2f%\n", ecal.time(),  ecal.time() / totlTime * 100);
	}
#endif

	return 0;
}

void DataSelection::fill() {
	if (isMultiTree == false) evTree->Fill();
	else {
		if (checkOption(DataSelection::LIST)) list.fill();
		if (checkOption(DataSelection::RTI)) rti.fill();
		if (checkOption(DataSelection::TRG)) trg.fill();
		if (checkOption(DataSelection::TOF)) tof.fill();
		if (checkOption(DataSelection::ACC)) acc.fill();
		if (checkOption(DataSelection::TRK)) trk.fill();
		if (checkOption(DataSelection::TRD)) trd.fill();
		if (checkOption(DataSelection::RICH)) rich.fill();
		if (checkOption(DataSelection::ECAL)) ecal.fill();
	}
}

int DataSelection::preselectEvent(AMSEventR * event, const std::string& officialDir) {
	if (event == nullptr) return -1;
	EventList::Weight = 1.;
    recEv.initTrRft();

	// Resolution tuning
	if (EventBase::checkEventMode(EventBase::ISS)) {
        TrLinearEtaDB::SetLinearCluster(); // Enable new Eta uniformity(Z=1-26 and above)
        TRFITFFKEY.Zshift = -1; // Enable the dZ correction
    }
    else if (EventBase::checkEventMode(EventBase::MC)) {
        TrExtAlignDB::SmearExtAlign();
        TRCLFFKEY.UseSensorAlign = 0;
        TRCLFFKEY.ClusterCofGOpt = 1;
        TRFITFFKEY.Zshift = -1; // // Disable the dZ correction
	}

	//-----------------------------//
	//----  Fast Preselection  ----//
	//-----------------------------//
	//COUT("============= <Run %u Event %u> =============\n", event->Run(), event->Event());

	// ~1~ (Based on BetaH(Beta))
	TofRecH::BuildOpt = 0; // normal

	// ~2~ (Based on TrTrack)
	if (event->NTrTrack() != 1) return -2001;

	// ~3~ (Based on TrdTrack)
	if (!(event->NTrdTrack() == 1 || event->NTrdHTrack() == 1)) return -3001;

	// ~4~ (Based on Particle)
	ParticleR  * partSIG = (event->NParticle() > 0) ? event->pParticle(0) : nullptr;
	TrTrackR   * trtkSIG = (partSIG != nullptr) ? partSIG->pTrTrack() : nullptr;
	BetaHR     * btahSIG = (partSIG != nullptr) ? partSIG->pBetaH()   : nullptr;
	TrdTrackR  * trdSIG  = (partSIG != nullptr) ? partSIG->pTrdTrack()  : nullptr;
	TrdHTrackR * trdhSIG = (partSIG != nullptr) ? partSIG->pTrdHTrack() : nullptr;
	if (partSIG == nullptr) return -4001;
	if (trtkSIG == nullptr || btahSIG == nullptr) return -4002;
	if ( trdSIG == nullptr && trdhSIG == nullptr) return -4003;

	// ~5~ (Based on BetaH)
	if (event->NBetaH() != 1) return -5001;
	if (btahSIG->GetBetaPattern() != 4444) return -5002;
	double betah = btahSIG->GetBeta();
    short  btahSign = MGNumc::Compare(betah);

	// ~6~ (Based on Track Hits)
	const unsigned short TrPtL34 =  12; //  4 +   8
	const unsigned short TrPtL56 =  48; // 16 +  32
	const unsigned short TrPtL78 = 192; // 64 + 128
	unsigned short trBitPattJ   = trtkSIG->GetBitPatternJ();
	unsigned short trBitPattXYJ = trtkSIG->GetBitPatternXYJ();
	
	bool isTrInner  = ((trBitPattJ&TrPtL34) > 0 && 
	                   (trBitPattJ&TrPtL56) > 0 && 
					   (trBitPattJ&TrPtL78) > 0);
	if (!isTrInner) return -6001;
	bool hasTrPtL1 = ((trBitPattJ&  1) > 0);
	bool hasTrPtL9 = ((trBitPattJ&256) > 0);
	
	Int_t numOfTrInX = 0;
	Int_t numOfTrInY = 0;
	for (Int_t ilay = 2; ilay <= 7; ilay++) {
		bool hasY  = ((trBitPattJ&(1<<ilay)) > 0);
		bool hasXY = ((trBitPattXYJ&(1<<ilay)) > 0);
		if (hasY)  numOfTrInY++;
		if (hasXY) numOfTrInX++;
	}
	if (numOfTrInX <= 2 || numOfTrInY <= 3) return -6002;
	
    int fitidMax = trtkSIG->iTrTrackPar(1, 0, 23);
    int fitidInn = trtkSIG->iTrTrackPar(1, 3, recEv.trRft(0, 0));
	if (fitidInn < 0) return -6003;
		
    double trRin = trtkSIG->GetRigidity(fitidInn);
    double trQin = trtkSIG->GetInnerQ_all(std::fabs(betah), fitidInn).Mean;
	if (MGNumc::Compare(trQin) <= 0) return -6004;

    // ~7~ (Scale Events by Track)
    if (EventBase::checkEventMode(EventBase::ISS) && MGNumc::Compare(trRin) > 0) {
        int fitidIn = trtkSIG->iTrTrackPar(1, 3, recEv.trRft(0, 0));
        int fitidL1 = trtkSIG->iTrTrackPar(1, 5, recEv.trRft(0, 1));
        int fitidL9 = trtkSIG->iTrTrackPar(1, 6, recEv.trRft(0, 2));
        int fitidFs = trtkSIG->iTrTrackPar(1, 7, recEv.trRft(0, 3));
        
        bool   isPosRig = true;
	    if (fitidIn >= 0 && trtkSIG->GetRigidity(fitidIn) < 0) isPosRig = false;
	    if (fitidL1 >= 0 && trtkSIG->GetRigidity(fitidL1) < 0) isPosRig = false;
	    if (fitidL9 >= 0 && trtkSIG->GetRigidity(fitidL9) < 0) isPosRig = false;
	    if (fitidFs >= 0 && trtkSIG->GetRigidity(fitidFs) < 0) isPosRig = false;

        if (isPosRig) { // Scale Events by Positive Rigidity
            double maxR = 0;
            if (fitidIn >= 0) maxR = std::max(maxR, trtkSIG->GetRigidity(fitidIn));
            if (fitidL1 >= 0) maxR = std::max(maxR, trtkSIG->GetRigidity(fitidL1));
            if (fitidL9 >= 0) maxR = std::max(maxR, trtkSIG->GetRigidity(fitidL9));
            if (fitidFs >= 0) maxR = std::max(maxR, trtkSIG->GetRigidity(fitidFs));
	    	
            double survivorProb = gScaleFunc1D.Eval(maxR);
            //double survivorProb = gScaleFunc2D.Eval(maxR, trQin);
	        if (MGNumc::Compare(MGRndm::DecimalUniform(), survivorProb) > 0) return -8001;
	        else EventList::Weight *= (1. / survivorProb);
        }
    }
	
    // ~8~ (Based on RTI)
	if (EventBase::checkEventMode(EventBase::ISS) && checkOption(DataSelection::RTI)) {
        if (!rti.processEvent(event)) return -8001;

	    const double cfSF = 0.8;
        double minStormer = *std::min_element(rti.fRti.cfStormer, rti.fRti.cfStormer+4);
		double minIGRF    = *std::min_element(rti.fRti.cfIGRF, rti.fRti.cfIGRF+4);
		double minCf      = cfSF * std::min(minStormer, minIGRF);

        int fitidIn = trtkSIG->iTrTrackPar(1, 3, recEv.trRft(0, 0));
        int fitidL1 = trtkSIG->iTrTrackPar(1, 5, recEv.trRft(0, 1));
        int fitidL9 = trtkSIG->iTrTrackPar(1, 6, recEv.trRft(0, 2));
        int fitidFs = trtkSIG->iTrTrackPar(1, 7, recEv.trRft(0, 3));

        double maxR = 0.0;
        if (fitidIn >= 0) maxR = std::max(maxR, std::fabs(trtkSIG->GetRigidity(fitidIn)));
        if (fitidL1 >= 0) maxR = std::max(maxR, std::fabs(trtkSIG->GetRigidity(fitidL1)));
        if (fitidL9 >= 0) maxR = std::max(maxR, std::fabs(trtkSIG->GetRigidity(fitidL9)));
        if (fitidFs >= 0) maxR = std::max(maxR, std::fabs(trtkSIG->GetRigidity(fitidFs)));
        if (maxR < cfSF) maxR = cfSF; // [GV]
    
		if (maxR < minCf) return -8002;
    }

    //--------------------------//
	//----  Reconstruction  ----//
	//--------------------------//
	if (!recEv.rebuild(event)) return -9999;

	return 0;
}

int DataSelection::selectEvent(AMSEventR * event) {
	if (event == nullptr) return -1;

	// User Define

	return 0;
}

int DataSelection::analysisEvent(AMSEventR * event) {
	if (event == nullptr) return -1;

	// User Define

	return 0;
}


//---- RunTagOperator ----//
RunTagOperator::RunTagOperator() { init(); }

RunTagOperator::~RunTagOperator() { init(); }

void RunTagOperator::init() {
	fRunTag.clear();
}

bool RunTagOperator::processEvent(AMSEventR * event, AMSChain * chain) {
	if (chain == nullptr) return false;
	TFile * file = chain->GetFile();
	TString filePath = TString(file->GetName());
	if (event == nullptr) return false;
	UInt_t runID   = event->Run();
	UInt_t eventID = event->Event();
		
	std::map<UInt_t, RunTagInfo>::iterator it = fRunTag.find(runID);
	if (it == fRunTag.end()) {
		RunTagInfo info;
		info.run = runID;
		info.eventFT = eventID;
		info.eventLT = eventID;
		info.numOfSelEvent = 1;
		info.file.push_back(filePath);
		
		if (EventBase::checkEventMode(EventBase::ISS)) {
			MGClock::MTime * mtime = MGClock::ConvertFromUTimeToMTime(runID, MGClock::ClockType::UTC);
			info.dateUTC = (mtime->tm_year + 1900) * 10000 + (mtime->tm_mon+1) * 100 + (mtime->tm_mday);
			info.timeUTC = (mtime->tm_hour) * 10000 + (mtime->tm_min) * 100 + (mtime->tm_sec);
		}
		
		fRunTag[runID] = info;
	}
	else {
		RunTagInfo & info = it->second;
		if (info.eventFT > eventID) info.eventFT = eventID;
		if (info.eventLT < eventID) info.eventLT = eventID;
		if (std::find(info.file.begin(), info.file.end(), filePath) == info.file.end()) info.file.push_back(filePath);
		info.numOfSelEvent++;
	}
	return true;
}

void RunTagOperator::save(TFile * file) {
	if (file == nullptr) return;
	file->cd();
	TTree * tree = new TTree("runTag", "RunTag information");
	RunTagInfo info;
	tree->Branch("runTag", &info);
	for (std::map<UInt_t, RunTagInfo>::iterator it = fRunTag.begin(); it != fRunTag.end(); ++it) {
		info = it->second;
		UInt_t nTrgEv = UInt_t( (info.numOfSelEvent == 1) ? 2 * info.eventFT : ((double(info.numOfSelEvent) / double(info.numOfSelEvent - 1)) * double(info.eventLT - info.eventFT)) );
		info.numOfTrgEvent = nTrgEv;
		tree->Fill();
	}
	file->cd();
}


//---- YiNtuple ----//
YiNtuple::MODE YiNtuple::selectionMode = YiNtuple::NORM;

YiNtuple::YiNtuple() {
#if Debug == true
	std::cerr << "Debug : Now, YiNtuple::YiNtuple()\n";
#endif
	fStopwatch.start();
}

YiNtuple::~YiNtuple() {
#if Debug == true
	std::cerr << "Debug : Now, YiNtuple::~YiNtuple()\n";
#endif
	init();

	fStopwatch.stop();
	fStopwatch.print();
}

inline void YiNtuple::init() {
#if Debug == true
	std::cerr << "Debug : Now, YiNtuple::init()\n";
#endif

	fGroup.first = 0;
	fGroup.second = -1;
	fFileList.clear();
	fFileDir = "";
	if (fChain != 0) delete fChain;
	fChain = 0;
	fFileName = "";
}

void YiNtuple::setOutputFile(const std::string& file_name, const std::string& path, bool isMultiTree) {
#if Debug == true
	std::cerr << "Debug : Now, YiNtuple::setOutputFile()\n";
#endif

	fFileName = std::string(path) + "/" + std::string(file_name);
	fData = new DataSelection();
	fData->setMultiTree(isMultiTree);
	fRunTagOp = new RunTagOperator;
}

void YiNtuple::readDataFrom(const std::string& file_list, Long64_t group_id, Long64_t group_size) {
#if Debug == true
	std::cerr << "Debug : Now, YiNtuple::readDataFrom()\n";
#endif

	COUT("\n**--------------------------------------------**\n");
	COUT("\n**    Read Data Form Source File List Info    **\n");
	COUT("\n**--------------------------------------------**\n");

	// start check sourceFileList.txt
	std::vector<std::string>&& flist = MGIO::ReadFileContent(file_list);
	if (flist.size() == 0)
		MGSys::ShowErrorAndExit(LOCADR(), "ROOT file list cannot be opend! Exiting ...");
	// end check sourceFileList.txt

	// start load data with group
	if (group_id == 0 && group_size == -1) group_size = flist.size();
	if (group_size <= 0 || group_size > flist.size() || group_id < 0 || group_id >= flist.size())
		MGSys::ShowErrorAndExit(LOCADR(), "Group format has error(1)! Exiting ...");
	
	Long64_t begin = group_id * group_size;
	Long64_t end   = (group_id + 1) * group_size;
	if (begin >= 0 && begin < flist.size() && end > flist.size()) {
		end = flist.size();
	}
	else if (begin < 0 || begin >= flist.size() || end < 1 || end > flist.size())
		MGSys::ShowErrorAndExit(LOCADR(), "ERROR : Group format has error(2)! Exiting ...");

	fGroup = std::make_pair(group_id, group_size);
	for (int it = begin; it < end; it++) {
		fFileList.push_back(flist.at(it));
	}

	COUT("\n---- Loading Root Files ----\n");
	COUT("Group : %ld th   [%ld files/group],    Total of Load Files : %ld \n", fGroup.first, fGroup.second, fFileList.size());
	for (Long64_t  it = 0; it < fFileList.size(); it++) {
		COUT("    Number : %ld,   %s\n", it, fFileList.at(it).c_str());
	}

	if (fFileList.size() != 0) {
		std::vector<std::string> && strs = MGRegex::Split(fFileList.at(0), MGRegex::Formula::kSlash);
		if (strs.size() > 2) fFileDir = strs.at(strs.size()-2);
	}
	// end load data with group

	// start read source file list
	bool stagedonly = true;
	unsigned int timeout = 10;
	fChain = new AMSChain("AMSRoot");
	int fileStatus = fChain->AddFromFile(file_list.c_str(), begin, end, stagedonly, timeout);
	if (fileStatus == -1)
		MGSys::ShowErrorAndExit(LOCADR(), "ROOT file list cannot be opend! Exiting ...");

	COUT("FileStatus : %d\n", fileStatus);
	COUT("Totally : %ld data events.\n", fChain->GetEntries());
	// end read source file list

	COUT("\n**-------------------------------------------**\n");
	COUT("\n**    Read Data Form Source File List End    **\n");
	COUT("\n**-------------------------------------------**\n");
}

void YiNtuple::saveInputFileList(TFile * file) {
	if (file == nullptr || fFileList.size() == 0) return;
	file->cd();
	TTree * tree = new TTree("fileList", "List Of Input File Info");
	TString filePath;
	tree->Branch("file", &filePath);
	for (int i = 0; i < fFileList.size(); ++i) {
		filePath = TString(fFileList.at(i).c_str());
		tree->Fill();
	}
	file->cd();
}

void YiNtuple::loopEventChain() {
	COUT("\n**-----------------------------**\n");
	COUT("\n**    Loop Event Chain Info    **\n");
	COUT("\n**-----------------------------**\n");

	TFile * file = 0;
	if (YiNtuple::checkSelectionMode(YiNtuple::NORM)) {
		file = new TFile(fFileName.c_str(), "RECREATE");
		fData->setEventTree();
	}
	else if (YiNtuple::checkSelectionMode(YiNtuple::COPY)) {
		fChain->OpenOutputFile(fFileName.c_str());
	}

	fData->setEnvironment(); // it must be before event loop. (before get event !)

	// check event type
	if (fChain->GetEntries() <= 0)
		MGSys::ShowErrorAndExit(LOCADR(), "Don't have event! Exiting ...");

	AMSEventR * ev = fChain->GetEvent(0);

	if (ev != 0) {
		bool isMC  = ev->nMCEventg() > 0 &&
			EventBase::checkEventMode(EventBase::MC);
		bool isBT  = (ev->nMCEventg() <= 0) &&
			(ev->Run() < 1305795600) &&
			EventBase::checkEventMode(EventBase::BT);
		bool isISS = (ev->nMCEventg() <= 0) &&
			(ev->Run() > 1305795600) &&
			EventBase::checkEventMode(EventBase::ISS);
		if (!isMC && !isBT && !isISS)
			MGSys::ShowErrorAndExit(LOCADR(), "Event type (ISS, BT, MC) is failed! Exiting ...");
	}

	Long64_t loop_entries = fChain->GetEntries();
	Long64_t first_entry = 0;
	Long64_t last_entry = loop_entries;

	Long64_t npassed = 0;
	Long64_t nprocessed = 0;
	const Long64_t printLimit = 2000;
	Long64_t printRate = loop_entries / 200;
	if (printRate < printLimit) printRate = printLimit;
	if (printRate > printLimit * 20) printRate = printLimit * 20;
	for (Long64_t ientry = first_entry; ientry < last_entry; ++ientry){
		if (nprocessed%printRate == 0) {
			fStopwatch.stop();

			const unsigned int MemSize = 1024;
			ProcInfo_t procinfo;
			gSystem->GetProcInfo(&procinfo);
			Long64_t memRes = procinfo.fMemResident / MemSize;
			Long64_t memVrl = procinfo.fMemVirtual  / MemSize;
			
			COUT("Info :: %lf %\n", 100. * float(nprocessed)/float(loop_entries));
			COUT("        Processed       : %ld / %ld\n", nprocessed, loop_entries);
			COUT("        Passed          : %ld / %ld\n", npassed, nprocessed);
			COUT("        Passed Ratio    : %lf %\n", ((nprocessed == 0) ? 0. : (100. * float(npassed)/float(nprocessed))));
			COUT("        Real Time       : %9.2f (second)\n", fStopwatch.time());
			COUT("        Processed Rate  : %8.2f (Hz)\n", nprocessed / fStopwatch.time());
			COUT("        Cpu    System   : %4.1f %\n", procinfo.fCpuSys);
			COUT("               User     : %4.1f %\n", procinfo.fCpuUser);
			COUT("        Memory Resident : %2ld GB %4ld MB\n", memRes / MemSize, memRes % MemSize);
			COUT("               Virtual  : %2ld GB %4ld MB\n", memVrl / MemSize, memVrl % MemSize);
		}
		nprocessed++;

		AMSEventR * event = fChain->GetEvent(ientry);
	
		//if (nprocessed > 1000) break; // testcode
		
		fRunTagOp->processEvent(event, fChain);

		int preselectEventStatus = fData->preselectEvent(event, fFileDir);
		if (preselectEventStatus < 0) continue;

		if (YiNtuple::checkSelectionMode(YiNtuple::NORM)) {
			int processEventStatus = fData->processEvent(event, fChain);
			if (processEventStatus < 0) continue;

			int selectEventStatus = fData->selectEvent(event);
			if (selectEventStatus < 0) continue;

			int analysisEventStatus = fData->analysisEvent(event);
			if (analysisEventStatus < 0) continue;

			fData->fill();
		}
		else if (YiNtuple::checkSelectionMode(YiNtuple::COPY)) {
			fChain->SaveCurrentEvent();
		}
		
		npassed++;
	}

	if (nprocessed == loop_entries) {
		fStopwatch.stop();
		const unsigned int MemSize = 1024;
		ProcInfo_t procinfo;
		gSystem->GetProcInfo(&procinfo);
		Long64_t memRes = procinfo.fMemResident / MemSize;
		Long64_t memVrl = procinfo.fMemVirtual  / MemSize;
		
		COUT("Info :: %lf %\n", 100. * float(nprocessed)/float(loop_entries));
		COUT("        Processed       : %ld / %ld\n", nprocessed, loop_entries);
		COUT("        Passed          : %ld / %ld\n", npassed, nprocessed);
		COUT("        Passed Ratio    : %lf %\n", ((nprocessed == 0) ? 0. : (100. * float(npassed)/float(nprocessed))));
		COUT("        Real Time       : %9.2f (second)\n", fStopwatch.time());
		COUT("        Processed Rate  : %8.2f (Hz)\n", nprocessed / fStopwatch.time());
		COUT("        Cpu    System   : %4.1f %\n", procinfo.fCpuSys);
		COUT("               User     : %4.1f %\n", procinfo.fCpuUser);
		COUT("        Memory Resident : %2ld GB %4ld MB\n", memRes / MemSize, memRes % MemSize);
		COUT("               Virtual  : %2ld GB %4ld MB\n", memVrl / MemSize, memVrl % MemSize);
		COUT("Info :: AMSRoot Files Processed Successfully Finished.\n");
	}
	else {
		COUT("Info :: AMSRoot Files Processed Seems Failed.\n");
		COUT("        Processed %ld in %ld\n", nprocessed, loop_entries);
	}

	if (YiNtuple::checkSelectionMode(YiNtuple::NORM)) {
		fRunTagOp->save(file);
		saveInputFileList(file);
		
		file->cd();
		file->Write();
		file->Close();
		if (file) delete file;
	}
	else if (YiNtuple::checkSelectionMode(YiNtuple::COPY)) {
		fChain->CloseOutputFile();
	}

	COUT("\n**----------------------------**\n");
	COUT("\n**    Loop Event Chain End    **\n");
	COUT("\n**----------------------------**\n");
}
#endif // __YiProdNtuple_TCC__
