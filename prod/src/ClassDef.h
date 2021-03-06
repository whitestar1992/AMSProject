/**********************************************************
 * Author        : Hsin-Yi Chou
 * Email         : hchou@cern.ch
 * Last modified : 2015-07-22 16:29
 * Filename      : ClassDef.h
 * Description   :
 * *******************************************************/
#ifndef __ClassDef_H__
#define __ClassDef_H__

#include <iostream>
#include <vector>
#include <TObject.h>
#include <TString.h>

// RunTagInfo
class RunTagInfo : public TObject {
	public :
		RunTagInfo() { init(); }
		~RunTagInfo() {}

		void init() {
			run = 0;
			eventFT = 0;
			eventLT = 0;
			numOfSelEvent = 0;
			numOfTrgEvent = 0;
			file.clear();
			
			dateUTC = 0;
			timeUTC = 0;
		}

	public :
		UInt_t run;
		UInt_t eventFT;            // first trigger event
		UInt_t eventLT;            // last trigger event
		UInt_t numOfSelEvent;      // number of select  events
		UInt_t numOfTrgEvent;      // number of trigger events
		std::vector<TString> file; // file list

		UInt_t  dateUTC;           // UTC date
		UInt_t  timeUTC;           // UTC time

	ClassDef(RunTagInfo, 6)
};


// SegPARTMCInfo
class SegPARTMCInfo : public TObject {
	public :
		SegPARTMCInfo() { init(); }
		~SegPARTMCInfo() {}

		void init() {
			dec = -1;
			lay = -1;
			bta = -1;
			mom = -1;
			std::fill_n(coo, 3, 0);
			std::fill_n(dir, 3, 0);
		}

	public :
        Short_t dec;    // [0] Silicon  [1] TOF  [2] TRD  [3] ECAL
        Short_t lay;
		Float_t bta;
		Float_t mom;
		Float_t coo[3];
		Float_t dir[3];
	
    ClassDef(SegPARTMCInfo, 2)
};

struct SegPARTMCInfo_sort {
	bool operator() (const SegPARTMCInfo & hit1, const SegPARTMCInfo & hit2) {
		if      (hit1.dec < hit2.dec) return true;
		else if (hit1.dec > hit2.dec) return false;
		else {
			if      (hit1.lay < hit2.lay) return true;
			else if (hit1.lay > hit2.lay) return false;
		}
		return false;
	}
};


// HitTRKMCInfo
class HitTRKMCInfo : public TObject {
	public :
		HitTRKMCInfo() { init(); }
		~HitTRKMCInfo() {}

		void init() {
			layJ = 0;
			tkid = 0;
			edep = -1;
			mom  = -1;
			std::fill_n(coo, 3, 0);
            std::fill_n(smr, 2, 0);
			std::fill_n(loc, 2, -1);
		}

	public :
		Short_t layJ;    // layerJ
		Short_t tkid;    // tkID
		Float_t edep;    // edep
		Float_t mom;     // momentum
		Float_t coo[3];
        Float_t smr[2];
        Float_t loc[2];

	ClassDef(HitTRKMCInfo, 6)
};

struct HitTRKMCInfo_sort {
	bool operator() (const HitTRKMCInfo & hit1, const HitTRKMCInfo & hit2) {
		if      (hit1.layJ < hit2.layJ) return true;
		else if (hit1.layJ > hit2.layJ) return false;
		else {
			if      (hit1.mom < hit2.mom) return true;
			else if (hit1.mom > hit2.mom) return false;
		}
		return false;
	}
};


// HitTRDMCInfo
class HitTRDMCInfo : public TObject {
	public :
		HitTRDMCInfo() { init(); }
		~HitTRDMCInfo() {}

		void init() {
			lay     = 0;
			sub     = 0;
			edep    = -1;
			std::fill_n(coo, 3, 0);
		}

	public :
		Short_t lay;     // layer
		Short_t sub;     // Ladder * 100 + Tube
		Float_t edep;    // (elc) edep
		Float_t coo[3];

	ClassDef(HitTRDMCInfo, 3)
};

struct HitTRDMCInfo_sort {
	bool operator() (const HitTRDMCInfo & hit1, const HitTRDMCInfo & hit2) {
		if      (hit1.lay < hit2.lay) return true;
		else if (hit1.lay > hit2.lay) return false;
		else {
			if      (hit1.sub < hit2.sub) return true;
			else if (hit1.sub > hit2.sub) return false;
			else {
				if      (hit1.edep < hit2.edep) return true;
				else if (hit1.edep > hit2.edep) return false;
			}
		}
		return false;
	}
};


// PartMCInfo
class PartMCInfo : public TObject {
	public :
		PartMCInfo() { init(); }
		~PartMCInfo() {}

		void init() {
			partID = 0;
			chrg   = 0;
			mass   = 0;
            beta   = 0;
			mom    = 0;
			ke     = 0;
			std::fill_n(coo, 3, 0);
			std::fill_n(dir, 3, 0);
            segs.clear();
			hits.clear();
		}

	public :
		Short_t partID;
		Float_t chrg;
		Float_t mass;
        Float_t beta;
		Float_t mom;
		Float_t ke;    // kinetic energy
		Float_t coo[3];
		Float_t dir[3];

        std::vector<SegPARTMCInfo> segs;
		std::vector<HitTRKMCInfo>  hits;

	ClassDef(PartMCInfo, 8)
};

struct PartMCInfo_sort {
	bool operator() (const PartMCInfo & par1, const PartMCInfo & par2) {
		if      (par1.coo[2] > par2.coo[2]) return true;
		else if (par1.coo[2] < par2.coo[2]) return false;
		else {
			if      (par1.partID < par2.partID) return true;
			else if (par1.partID > par2.partID) return false;
			else {
				if      (par1.mom > par2.mom) return true;
				else if (par1.mom < par2.mom) return false;
			}
		}
		return false;
	}
};


// VertexMCInfo
class VertexMCInfo : public TObject {
	public :
		VertexMCInfo() { init(); }
		~VertexMCInfo() {}

		void init() {
			status = false;
			std::fill_n(coo, 3, 0.);
            numOfPart = 0;
            partId = -1;
            partKe = -1;
		}

	public :
		Bool_t  status;
		Float_t coo[3];

        Short_t numOfPart;
        Short_t partId;
        Float_t partKe;
	
	ClassDef(VertexMCInfo, 7)
};


// HitTRKInfo
class HitTRKInfo : public TObject {
	public :
		HitTRKInfo() { init(); }
		~HitTRKInfo() {}

		void init() {
			std::fill_n(clsId,  2, -1);
			layJ =  0;
			tkid =  0;
			sens = -1;
			mult = -1;
			std::fill_n(side, 2, false);
			std::fill_n(coo, 3, 0);
			std::fill_n(adc, 2, -1);
			std::fill_n(loc, 2, -1);
			std::fill_n(nsr, 2, 0);
		}

	public :
		Short_t clsId[2]; // cluster id (x, y)
		Short_t layJ;     // layerJ
		Short_t tkid;     // tkID
		Short_t sens;     // sensor
		Short_t mult;     // multiplicity
		Bool_t  side[2];  // side, x, y
		Float_t coo[3];   // coordinate
		Float_t adc[2];   // (elc) signal
		Float_t loc[2];   // (elc) cofg loc
        Short_t nsr[2];   // (elc) n-stripe

	ClassDef(HitTRKInfo, 8)
};

struct HitTRKInfo_sort {
	bool operator() (const HitTRKInfo & hit1, const HitTRKInfo & hit2) {
		if      (hit1.layJ < hit2.layJ) return true;
		else if (hit1.layJ > hit2.layJ) return false;
		else {
			if      (hit1.tkid < hit2.tkid) return true;
			else if (hit1.tkid > hit2.tkid) return false;
		}
		return false;
	}
};


// HitTRDInfo
class HitTRDInfo : public TObject {
	public :
		HitTRDInfo() { init(); }
		~HitTRDInfo() {}

		void init() {
			lay  = 0;
			sub  = 0;
			side = 0;
			amp  = -1;
			std::fill_n(coo, 3, 0);
		}

	public :
		Short_t lay;    // layer
		Short_t sub;    // ladder * 100 + tube
		Short_t side;   // side, 1 x, 2 y, 3 xy
		Float_t amp;    // (elc) amp
		Float_t coo[3];

	ClassDef(HitTRDInfo, 2)
};

struct HitTRDInfo_sort {
	bool operator() (const HitTRDInfo & hit1, const HitTRDInfo & hit2) {
		if      (hit1.lay < hit2.lay) return true;
		else if (hit1.lay > hit2.lay) return false;
		else {
			if      (hit1.sub < hit2.sub) return true;
			else if (hit1.sub > hit2.sub) return false;
			else {
				if      (hit1.amp < hit2.amp) return true;
				else if (hit1.amp > hit2.amp) return false;
			}
		}
		return false;
	}
};


// HitRICHInfo
class HitRICHInfo : public TObject {
	public :
		HitRICHInfo() { init(); }
		~HitRICHInfo() {}

		void init() {
            status = 0;
            cross = false;
			channel = -1;
            npe = -1;
            
			std::fill_n(coo, 3, 0);
            dist = -1.;
            dtha = -1.;
            dphi = -1.;
            rtha = -1.;
            rphi = -1.;
		}

	public :
        Short_t status;
        Bool_t  cross;   // cross
		Short_t channel; // channel = 16 * PMT + pixel
        Float_t npe;     // ADC counts above the pedestal/gain of the channel.
		Float_t coo[3];  // x, y, z
        Float_t dist;    // distance to particle
        Float_t dtha;    // direct theta
        Float_t dphi;    // direct phi (pmt)
        Float_t rtha;    // reflect theta
        Float_t rphi;    // reflect phi (pmt)

	ClassDef(HitRICHInfo, 1)
};

struct HitRICHInfo_sort {
	bool operator() (const HitRICHInfo & hit1, const HitRICHInfo & hit2) {
		if      (hit1.channel < hit2.channel) return true;
		else if (hit1.channel > hit2.channel) return false;
		return false;
	}
};


// HitECALInfo
class HitECALInfo : public TObject {
	public :
		HitECALInfo() { init(); }
		~HitECALInfo() {}

		void init() {
			id   = 0;
			side = 0;
			edep = -1;
			std::fill_n(coo, 3, 0);
		}

	public :
		Short_t id;     // plane * 100 + cell
		Bool_t  side;   // side 0 x, 1 y
		Float_t edep;   // (phy) edep
		Float_t coo[3]; // x, y, z

	ClassDef(HitECALInfo, 2)
};

struct HitECALInfo_sort {
	bool operator() (const HitECALInfo & hit1, const HitECALInfo & hit2) {
		if      (hit1.id < hit2.id) return true;
		else if (hit1.id > hit2.id) return false;
		return false;
	}
};


// ClsACCInfo
class ClsACCInfo : public TObject {
	public :
		ClsACCInfo() { init(); }
		~ClsACCInfo() {}

		void init() {
			sector = 0;
			time = 0;
			rawQ = -1;
			std::fill_n(coo, 3, 0);
		}

	public :
		Short_t sector; // sector (1~8)
		Float_t time;   // time
		Float_t rawQ;   // raw Charge
		Float_t coo[3]; // x, y, z

	ClassDef(ClsACCInfo, 1)
};

struct ClsACCInfo_sort {
	bool operator() (const ClsACCInfo & cls1, const ClsACCInfo & cls2) {
		return (cls1.time < cls2.time);
	}
};


// TrackInfo
class TrackInfo : public TObject {
	public :
		TrackInfo() { init(); }
		~TrackInfo() {}

		void init() {
			bitPattJ = 0;
			bitPattXYJ = 0;
			bitPatt = 0;
			bitPattXY = 0;
			QIn = -1;
			QL2 = -1;
			QL1 = -1;
			QL9 = -1;
		
            constexpr int naglo = 3;
			std::fill_n(status[0], naglo * 4, false);
			std::fill_n(rig[0], naglo * 4, 0);
			std::fill_n(chisq[0][0], naglo * 4 * 3, -1);
			std::fill_n(stateLJ[0][0][0], naglo * 4 * 9 * 7, 0);
			
            std::fill_n(cpuTime[0], naglo * 4, -1);
			
			hits.clear();
		}

	public :
		UShort_t bitPattJ;
		UShort_t bitPattXYJ;
		
		// bitPatt   := hasInner   * 1 + hasL2   * 2 + hasL1   * 4 + hasL9   * 8
		// bitPattXY := hasInnerXY * 1 + hasL2XY * 2 + hasL1XY * 4 + hasL9XY * 8
		// Inner    (XY) := ((bitPattXY&  1)==  1)
		// InnerL1  (XY) := ((bitPattXY&  5)==  5)
		// InnerL9  (XY) := ((bitPattXY&  9)==  9)
		// FullSpan (XY) := ((bitPattXY& 13)== 13)
		Short_t bitPatt;
		Short_t bitPattXY;

		// Track Charge
		Float_t QIn;
		Float_t QL1;
		Float_t QL2;
		Float_t QL9;

		// Algorithm     (CHOUTKO, KALMAN, HCHOU)
		// Track Pattern (Inn, InnL1, InnL9, FS)
		Bool_t  status[3][4];
		Float_t rig[3][4];
		Float_t chisq[3][4][3]; // normalized chisq (X, Y, XY)
		Float_t stateLJ[3][4][9][7]; // track state at ecah layer (x y z dirx diry dirz rig)
        
        Float_t cpuTime[3][4]; // [ms]
	
		// Track Hits
		std::vector<HitTRKInfo> hits;

	ClassDef(TrackInfo, 8)
};


// VertexInfo
class VertexInfo : public TObject {
	public :
		VertexInfo() { init(); }
		~VertexInfo() {}

		void init() {
			std::fill_n(vtxZqu, 5, 0.0);
			std::fill_n(vtxR, 8, 0.0);
			vtxR[7] = -1.0;
			trackID.clear();
		}

	public :
		Float_t vtxZqu[5]; // vertex (coo, dx, dy) based on Zqu
		Float_t vtxR[8]; // vertex (coo, dir, mom, chi/ndf) based on VertexR
		std::vector<Int_t> trackID;
	
	ClassDef(VertexInfo, 2)
};


// ShowerInfo
class ShowerInfo : public TObject {
	public :
		ShowerInfo() { init(); }
		~ShowerInfo() {}
		
		void init() {
			energyD = -1;
			energyE = -1;
			PisaBDT = -2;
			Q = -1;

			hadronApex = -1;
			hadronEnergy = -1;
		}

	public :
		Float_t energyD;  // energy deposit [GeV]
		Float_t energyE;  // Pisa
		Float_t PisaBDT;  // Pisa
		Float_t Q;        // ecal charge

		Short_t hadronApex; // reject high-apex events (say, Apex > 12)
		Float_t hadronEnergy;

		ClassDef(ShowerInfo, 4)
};


// LIST
class LIST : public TObject {
	public :
		LIST() { init(); }
		~LIST() {}

		void init() {
			run    = 0;
			event  = 0;
			entry  = 0;
			weight = 1;
		}

	public :
		UInt_t   run;
		UInt_t   event;
		UInt_t   entry;
		Float_t  weight;

	ClassDef(LIST, 2)
};


// G4MC
class G4MC : public TObject {
	public :
		G4MC() { init(); }
		~G4MC() {}

		void init() {
			beamID = -1;

			primPart.init();
			primVtx.init();
		}

	public :
		Short_t      beamID; // only for MC Beam Test (400GeV proton)
		PartMCInfo   primPart;
		VertexMCInfo primVtx;

	ClassDef(G4MC, 6)
};


// RTI
class RTI : public TObject {
	public :
		RTI() { init(); }
		~RTI() {}

		void init() {
			flagRun = true;
			isGoodSecond = true;
			zenith = -1;
			std::fill_n(cfStormer, 4, -1);
			std::fill_n(cfIGRF, 4, -1);
			radiusGTOD = -1;
			thetaGTOD = -99;
			phiGTOD = -99;
		    latMAG = -99;
			longMAG = -99;
			latGAT = -99;
			longGAT = -99;
			std::fill_n(rptISS, 3, 0);
			std::fill_n(velISS, 3, 0);
			std::fill_n(yprISS, 3, 0);
			isInSAA = false;
			uTime = 0;
			dateUTC = 0;
			timeUTC = 0;
			liveTime = -1;
			std::fill_n(trackerAlign[0], 4, 0);
			trackerTemp = 0;
			isInShadow  = -1;
		}

	public :
		Bool_t  flagRun;             // true, if good
		Bool_t  isGoodSecond;        // true, if good
		Float_t zenith;              // ams zenith (z-axis) angle [degrees]
		Float_t cfStormer[4];        // max Stormer cutoff for 25, 30, 35, 40 degrees [GeV]
		Float_t cfIGRF[4];           // max IGRF cutoff for 25, 30, 35, 40 degrees [GeV]
		Float_t radiusGTOD;          // distance from earth to ams [cm]
		Float_t thetaGTOD;           // earth coordinate [rad]
		Float_t phiGTOD;             // earth coordinate [rad]
		Float_t latMAG;              // geomagnetic latitude [rad]
		Float_t longMAG;             // geomagnetic longitude [rad]
		Float_t latGAT;              // ams pointing galatic latitude [rad]
		Float_t longGAT;             // ams pointing galatic longitude [rad]
		Float_t rptISS[3];           // ISS coordinates (R, Phi, Theta) (GTOD)
		Float_t velISS[3];           // ISS velocity (Vel rad/sec, VelPhi rad, VelTheta rad)
		Float_t yprISS[3];           // ISS attitude (Yaw, Pitch, Roll)
		Bool_t  isInSAA;             // true, if ams in south atlantic anomaly
		UInt_t  uTime;               // unix time
		UInt_t  dateUTC;             // UTC date
		UInt_t  timeUTC;             // UTC time
		Float_t liveTime;            // fraction of "non-busy" time
		Float_t trackerAlign[2][2];  // L1 x,y L9 x,y
		Float_t trackerTemp;         // tracker temperature (Sensor A)
		
		Short_t isInShadow;          // particle pass through the ISS Solar Array
		                             // return
									 //  -1, no particle information
									 //   0, not in shadow
									 //   1, in shadow

	ClassDef(RTI, 8)
};


// TRG
class TRG : public TObject {
	public :
		TRG() { init(); }
		~TRG() {}

		void init() {
			bit = 0;
			physicalPatt = -1;
			logicPatt = -1;
		}

	public :
		Short_t bit;          // level 1 trigger : bit := extTrg * 1 + unBiasTrgTOF * 2 + unBiasTrgECAL * 4 + physTrg * 8;
		Int_t   physicalPatt; // level 1 trigger : physical
		Int_t   logicPatt;    // level 1 trigger : logic

	ClassDef(TRG, 2)
};


// TOF
class TOF : public TObject {
	public :
		TOF() { init(); }
		~TOF() {}

		void init() {
			numOfCls = 0;
			numOfClsH = 0;
			numOfBeta = 0;
			numOfBetaH = 0;

			numOfInTimeCls = -1;

			statusBeta = false;
			beta = 0;

			statusBetaH = false;
			betaHBit = 0;
			betaHPatt = 0;
			betaH = 0;
			normChisqT = -1;
			normChisqC = -1;
			std::fill_n(coo[0], 4*3, 0);
			std::fill_n(err[0], 4*3, 0);
			std::fill_n(Q, 4, -1);
			Qall = -1;
            Zall = -1;

			std::fill_n(numOfExtCls, 4, 0);
		}

	public :
		Short_t numOfCls;
		Short_t numOfClsH;
		Short_t numOfBeta;
		Short_t numOfBetaH;

		Short_t numOfInTimeCls;

		Bool_t  statusBeta;
		Float_t beta;

		Bool_t  statusBetaH;
		Short_t betaHBit;
		Short_t betaHPatt;
		Float_t betaH;
		Float_t normChisqT;
		Float_t normChisqC;
        Float_t coo[4][3];
        Float_t err[4][3];
		Float_t Q[4];
		Float_t Qall;
        Short_t Zall;

		// extern clusters
		Short_t numOfExtCls[4];

	ClassDef(TOF, 9)
};


// ACC
class ACC : public TObject {
	public :
		ACC() { init(); }
		~ACC() {}

		void init() {
			numOfCls = 0;
			clusters.clear();
		}

	public :
		Short_t numOfCls;

		std::vector<ClsACCInfo> clusters;

	ClassDef(ACC, 2)
};


// TRK
class TRK : public TObject {
	public :
		TRK() { init(); }
		~TRK() {}

		void init() {
            numOfTrack = 0;

			track.init();

            ftL56Dist = -1;
            survHeL56Prob = -1;
            ratN10Smin = -1;
            std::fill_n(ratN10S, 7, -1);
            
            noiseInTrSH = -1;
            std::fill_n(betaSH, 3, -1);
            massEstSH = -99;
		}

	public :
        Short_t numOfTrack;

		TrackInfo track;

        // Haino's tools
        Float_t ftL56Dist;     // tracker feet (typical cut is ftL56Dist < 0.5~6)
        Float_t survHeL56Prob; // The variable you can play with on cuts (typical cut is hsv<0.26)
        Float_t ratN10Smin;    //
        Float_t ratN10S[7];    // a ratio of raw ADC used for the hit over the sum of n=10 strips around. (from L2 to L8)
        
        Short_t noiseInTrSH;   // keep (noiseInTrSH == 0) events
        Float_t betaSH[3];     // from dE/dx by 
                               // [0] Tracker
                               // [1] Tracker, TOF
                               // [2] Tracker, TOF and/or TRD
        Float_t massEstSH;     // mass estimator log-likelihood

	ClassDef(TRK, 6)
};


// TRD
class TRD : public TObject {
	public :
		TRD() { init(); }
		~TRD() {}

		void init() {
            numOfCls = 0;
            numOfSegment = 0;
			numOfTrack = 0;
			numOfHTrack = 0;

			std::fill_n(statusKCls, 2, false);
			std::fill_n(LLRep, 2, -1);
			std::fill_n(LLReh, 2, -1);
			std::fill_n(LLRph, 2, -1);
			std::fill_n(LLRnhit, 2, -1);
			std::fill_n(Q, 2, -1);

			trackStatus = false;
			std::fill_n(trackState, 6, 0);

            std::fill_n(vtxNum, 3, 0);
            vtxNTrk = 0;
            vtxNHit = 0;
            vtxChi2 = 0;
            std::fill_n(vtxCoo, 3, 0);
		}

	public :
        Short_t numOfCls;
        Short_t numOfSegment;
		Short_t numOfTrack;
		Short_t numOfHTrack;

		// (TrdHTrack or TrdTrack) and TrTrack
		Bool_t  statusKCls[2]; // true, rebuild success (Trd, Trk)
		Float_t LLRep[2];
		Float_t LLReh[2];
		Float_t LLRph[2];
		Short_t LLRnhit[2];
		Float_t Q[2];

		// TrdTrack or TrdHTrack (first TrdH, second Trd)
		Bool_t  trackStatus;
		Float_t trackState[6]; // coo, dir

        // TRDVertex
        Short_t vtxNum[3]; // (3d, 2d_y, 2d_x)
        Short_t vtxNTrk;
        Short_t vtxNHit;
        Float_t vtxChi2;
        Float_t vtxCoo[3];

	ClassDef(TRD, 7)
};


// RICH
class RICH : public TObject {
	public :
		RICH() { init(); }
		~RICH() {}

		void init() {
            numOfHit = 0;
            
            status = false;
			isGood = false;
            kind = -1;
            tile = -1;
			beta = -1;
			Q = -1;
            numOfExpPE = -1;
            eftOfColPE = -1;
			
            vetoKind = -1;
			vetoTile = -1;
			vetoRfrIndex = -1;
			vetoDistToBorder = -1;
			vetoIsInFiducialVolume = false;

            std::fill_n(vetoNumOfExpPE, 3, 0);
            
            vetoNumOfCrsHit = 0;
            std::fill_n(vetoNumOfCkvHit[0], 3*3, 0);
            
            hits.clear();
		}

	public :
        Short_t numOfHit;
        
        // Official RichRingR
		Bool_t  status;
		Bool_t  isGood;
        Short_t kind;
        Short_t tile;
		Float_t beta;
		Float_t Q;
        Float_t numOfExpPE;
        Float_t eftOfColPE;

		// Rich Veto
		Short_t vetoKind;          // -1, None, 0, Aerogel 1, NaF
		Short_t vetoTile;          // tile id
		Float_t vetoRfrIndex;      // refractive index
		Float_t vetoDistToBorder;  // dist To Border Edge
		Bool_t  vetoIsInFiducialVolume;
        
        // [0] electron [1] proton [2] deuterium
		Float_t vetoNumOfExpPE[3];     // number of photoelectrons expected for a given track, beta and charge.
        Short_t vetoNumOfCrsHit;       // Particle Cross PMT
        Short_t vetoNumOfCkvHit[3][3]; // Photo
                                       // [0] In  Ring
                                       // [1] Out Ring
                                       // [2] Non Physics

        // Rich Hits
        std::vector<HitRICHInfo> hits;
	
    ClassDef(RICH, 8)
};


// ECAL
class ECAL : public TObject {
	public :
		ECAL() { init(); }
		~ECAL() {}

		void init() {
			numOfShower = 0;
            shower.init();
		}

	public :
		Short_t    numOfShower;
		ShowerInfo shower;

	ClassDef(ECAL, 5)
};

#endif // __ClassDef_H__
