#ifndef __TRACKLibs_PhySt_H__
#define __TRACKLibs_PhySt_H__


namespace TrackSys {


class PhyArg {
    public :
        static void SetOpt(Bool_t opt_mscat = true, Bool_t opt_eloss = true) { opt_mscat_ = opt_mscat; opt_eloss_ = opt_eloss; }
        static void SetOptMscat(Bool_t opt_mscat = true) { opt_mscat_ = opt_mscat; }
        static void SetOptEloss(Bool_t opt_eloss = true) { opt_eloss_ = opt_eloss; }

        static const Bool_t& OptMscat() { return opt_mscat_; }
        static const Bool_t& OptEloss() { return opt_eloss_; }
   
    private :
        static Bool_t opt_mscat_;
        static Bool_t opt_eloss_;

    public :
        PhyArg(Bool_t sw_mscat = OptMscat(), Bool_t sw_eloss = OptEloss()) { reset(sw_mscat, sw_eloss); }
        ~PhyArg() {}

        inline void reset(Bool_t sw_mscat = OptMscat(), Bool_t sw_eloss = OptEloss()) { field_ = (sw_mscat || sw_eloss); sw_mscat_ = sw_mscat; sw_eloss_ = sw_eloss; mscatu_tau_ = 0.0; mscatu_rho_ = 0.0; mscatc_tau_ = 0.0; mscatc_rho_ = 0.0; eloss_ion_ = 0.0; eloss_brm_ = 0.0; tune_eloss_ion_ = MGMath::ONE; } 
        
        inline void set_mscat(Double_t tauu = 0.0, Double_t rhou = 0.0, Double_t tauc = 0.0, Double_t rhoc = 0.0) { if (sw_mscat_) {mscatu_tau_ = tauu; mscatu_rho_ = rhou; mscatc_tau_ = tauc; mscatc_rho_ = rhoc; } }
        
        inline void set_eloss(Double_t ion = 0.0, Double_t brm = 0.0, Double_t ionlmt = 0.0) { if (sw_eloss_) { eloss_ion_ = (ion<-std::max(LMT_SGM_, ionlmt)?-std::max(LMT_SGM_, ionlmt):ion); eloss_brm_ = ((brm<0.0)?0.0:brm); } }
        
        inline void set(Double_t tauu = 0.0, Double_t rhou = 0.0, Double_t tauc = 0.0, Double_t rhoc = 0.0, Double_t ion = 0.0, Double_t brm = 0.0) { set_mscat(tauu, rhou, tauc, rhoc); set_eloss(ion, brm); }

        void rndm_mscatu() { mscatu_tau_ = 0.0; mscatu_rho_ = 0.0; if (sw_mscat_) { mscatu_tau_ = pdf_mscat_.rndm(); mscatu_rho_ = pdf_mscat_.rndm(); } }
        void rndm_mscatc() { mscatc_tau_ = 0.0; mscatc_rho_ = 0.0; if (sw_mscat_) { mscatc_tau_ = pdf_mscat_.rndm(); mscatc_rho_ = pdf_mscat_.rndm(); } }
        void rndm_mscat() { rndm_mscatu(); rndm_mscatc(); }

        void rndm_eloss_ion(Double_t kpa = 0.0, Double_t mos = 0.0);
        void rndm_eloss_brm(Double_t nrl = 0.0) { if (sw_eloss_) { Double_t bremslen = nrl / MGMath::LOG_TWO; eloss_brm_=((bremslen<=0.0)?0.0:MGRndm::Gamma(bremslen,1.0/bremslen)()); } }
        void rndm_eloss(Double_t kpa = 0.0, Double_t mos = 0.0, Double_t nrl = 0.0) { rndm_eloss_ion(kpa, mos); rndm_eloss_brm(nrl); }

        void rndm(Double_t kpa = 0.0, Double_t mos = 0.0, Double_t nrl = 0.0) { rndm_mscatu(); rndm_mscatc(); rndm_eloss_ion(kpa, mos); rndm_eloss_brm(nrl); }

        inline const Bool_t& operator() () const { return field_; }

        inline const Bool_t& mscat() const { return sw_mscat_; }
        inline const Bool_t& eloss() const { return sw_eloss_; }

        inline const Double_t& tauu() const { return mscatu_tau_; }
        inline const Double_t& rhou() const { return mscatu_rho_; }
        inline const Double_t& tauc() const { return mscatc_tau_; }
        inline const Double_t& rhoc() const { return mscatc_rho_; }
        
        inline const Double_t& ion() const { return eloss_ion_; }
        inline const Double_t& brm() const { return eloss_brm_; }
       
        inline Double_t etauu() const { return pdf_mscat_.efft_sgm(mscatu_tau_); }
        inline Double_t erhou() const { return pdf_mscat_.efft_sgm(mscatu_rho_); }
        inline Double_t etauc() const { return pdf_mscat_.efft_sgm(mscatc_tau_); }
        inline Double_t erhoc() const { return pdf_mscat_.efft_sgm(mscatc_rho_); }
      
        // Note: Expert Tool
        inline void set_tune_ion(Double_t tune = 1.0) { if (sw_eloss_) { 
            if      (tune < MGMath::ZERO) tune_eloss_ion_ = MGMath::ZERO;
            else if (tune > MGMath::ONE)  tune_eloss_ion_ = MGMath::ONE;
            else                          tune_eloss_ion_ = tune;
        } }
        inline const Double_t& tune_ion() const { return tune_eloss_ion_; }

        inline static MultiGauss& pdf_mscat() { return pdf_mscat_; }

    private :
        Bool_t   field_;
        Bool_t   sw_mscat_;
        Bool_t   sw_eloss_;
        Double_t mscatu_tau_;
        Double_t mscatu_rho_;
        Double_t mscatc_tau_;
        Double_t mscatc_rho_;
        Double_t eloss_ion_;
        Double_t eloss_brm_;

        // Note: Expert Tool
        Double_t tune_eloss_ion_;
        
    protected :
        static MultiGauss pdf_mscat_;

    protected :
        static constexpr Int_t    NPX_      = 400;
        static constexpr Double_t LMT_SGM_  = 4.0;
        static constexpr Double_t STEP_KPA_ = 0.02;
        static constexpr Double_t STEP_MOS_ = 0.2;
        static std::map<std::pair<Int_t, Int_t>, TF1 *> pdf_eloss_ion_;
};

Bool_t PhyArg::opt_mscat_ = true;
Bool_t PhyArg::opt_eloss_ = true;

MultiGauss PhyArg::pdf_mscat_(8.931154e-01, 1.000000e+00, 9.211506e-02, 1.851060e+00, 1.167630e-02, 4.478370e+00, 3.093243e-03, 2.390957e+01);

std::map<std::pair<Int_t, Int_t>, TF1 *> PhyArg::pdf_eloss_ion_;


class VirtualPhySt {
    public :
        VirtualPhySt() { reset(); }
        ~VirtualPhySt() {}

        inline void reset(Bool_t field = false);
       
        inline const Bool_t& operator() () const { return field_; }

        inline void set_sign(Short_t sign = 1) { sign_ = ((sign>=0)?1:-1); }
    
        inline const Short_t& sign() { return sign_; }

        inline void set_len(Double_t len) { len_ = len; }
        inline void set_nrl(Double_t nrl) { nrl_ = nrl; }
        inline void set_ela(Double_t ela) { ela_ = ela; }
        
        inline const Double_t& len() const { return len_; }
        inline const Double_t& nrl() const { return nrl_; }
        inline const Double_t& ela() const { return ela_; }

        inline void set_orth(const SVecD<3>& tau, const SVecD<3>& rho) { tau_ = tau; rho_ = rho; }
        inline const SVecD<3>& tau() const { return tau_; }
        inline const SVecD<3>& rho() const { return rho_; }
        inline const Double_t& tau(Int_t idx) const { return tau_(idx); }
        inline const Double_t& rho(Int_t idx) const { return rho_(idx); }

        inline void set_mscatu(Double_t mscatu) { mscatu_ = mscatu; }
        inline void set_mscatc(Double_t mscatcu, Double_t mscatcl) { mscatcu_ = mscatcu; mscatcl_ = mscatcl; } 

        inline const Double_t& mscatu()  const { return mscatu_; }
        inline const Double_t& mscatcu() const { return mscatcu_; }
        inline const Double_t& mscatcl() const { return mscatcl_; }

        inline SVecD<3> symbk_mscatu(Double_t tauu = 0.0, Double_t rhou = 0.0) const { return (field_ ? ((tauu*mscatu_)*tau_ + (rhou*mscatu_)*rho_) : SVecD<3>()); }
        inline SVecD<3> symbk_mscatc(Double_t tauu = 0.0, Double_t rhou = 0.0, Double_t tauc = 0.0, Double_t rhoc = 0.0) const { return (field_ ? ((tauu*mscatcu_+tauc*mscatcl_)*tau_ + (rhou*mscatcu_+rhoc*mscatcl_)*rho_) : SVecD<3>()); }

        inline void set_eloss_ion(Double_t kpa, Double_t mpv, Double_t sgm) { eloss_ion_kpa_ = kpa; eloss_ion_mpv_ = mpv; eloss_ion_sgm_ = sgm; }
        inline void set_eloss_brm(Double_t men) { eloss_brm_men_ = men; }
        
        inline const Double_t& eloss_ion_kpa() const { return eloss_ion_kpa_; }
        inline const Double_t& eloss_ion_mpv() const { return eloss_ion_mpv_; }
        inline const Double_t& eloss_ion_sgm() const { return eloss_ion_sgm_; }
        inline Double_t        eloss_ion_mos() const { return ( MGNumc::EqualToZero(eloss_ion_sgm_) ? MGMath::ZERO : (eloss_ion_mpv_/eloss_ion_sgm_) ); }
        inline Double_t        eloss_ion_lmt() const { Double_t lmt = (eloss_ion_mpv_/(eloss_ion_kpa_*eloss_ion_sgm_)); return ((MGNumc::Valid(lmt) && MGNumc::Compare(lmt)>0) ? lmt : MGMath::ZERO); }

        inline const Double_t& eloss_brm_men() const { return eloss_brm_men_; }

        inline Double_t symbk_eloss(Double_t ion = 0.0, Double_t brm = 0.0) const { return (field_ ? (sign_ * (ion * (eloss_ion_kpa_*eloss_ion_sgm_) + brm * eloss_brm_men_)) : MGMath::ZERO); }

    private :
        Bool_t   field_;
        Short_t  sign_;

        Double_t len_;
        Double_t nrl_;
        Double_t ela_;

        SVecD<3> tau_;
        SVecD<3> rho_;

        Double_t mscatu_;
        Double_t mscatcu_;
        Double_t mscatcl_;

        Double_t eloss_ion_kpa_;
        Double_t eloss_ion_mpv_;
        Double_t eloss_ion_sgm_;

        Double_t eloss_brm_men_;
};


class PhySt {
    public :
        PhySt(const PartType& type = PartType::Proton, Bool_t sw_mscat = PhyArg::OptMscat(), Bool_t sw_eloss = PhyArg::OptEloss()) : arg_(sw_mscat, sw_eloss) { reset(type); }
        PhySt(const PartInfo& info, Bool_t sw_mscat = PhyArg::OptMscat(), Bool_t sw_eloss = PhyArg::OptEloss()) : arg_(sw_mscat, sw_eloss) { reset(info.type()); }
        ~PhySt() {}

        void reset(const PartType& type = PartType::Proton, Bool_t sw_mscat = PhyArg::OptMscat(), Bool_t sw_eloss = PhyArg::OptEloss());
        
        void set_state_with_cos(Double_t cx, Double_t cy, Double_t cz, Double_t ux = 0., Double_t uy = 0., Double_t uz = -1.);
        void set_state_with_tan(Double_t cx, Double_t cy, Double_t cz, Double_t tx = 0., Double_t ty = 0., Double_t uz = -1.);
        void set_state_with_uxy(Double_t cx, Double_t cy, Double_t cz, Double_t ux = 0., Double_t uy = 0., Short_t signz = -1);
        void set_state(Double_t cx, Double_t cy, Double_t cz, Double_t mx, Double_t my, Double_t mz);
        
        inline void set_state_with_cos(const SVecD<3>& c, const SVecD<3>& u = SVecD<3>(0., 0., -1.)) { set_state_with_cos(c(0), c(1), c(2), u(0), u(1), u(2)); }
        inline void set_state_with_tan(const SVecD<3>& c, const SVecD<3>& u = SVecD<3>(0., 0., -1.)) { set_state_with_tan(c(0), c(1), c(2), u(0), u(1), u(2)); }
        inline void set_state_with_uxy(const SVecD<3>& c, const SVecD<3>& u = SVecD<3>(0., 0., -1.)) { set_state_with_uxy(c(0), c(1), c(2), u(0), u(1), u(2)); }
        inline void set_state(const SVecD<3>& c, const SVecD<3>& m) { set_state(c(0), c(1), c(2), m(0), m(1), m(2)); }
        
        void set_mom(Double_t mom, Double_t sign = 0.);
        
        void set_eta(Double_t eta);
        
        void set_irig(Double_t irig);

        void set_rig(Double_t rig);

        void print() const;

        inline const PartInfo& info() const { return info_; }
        inline const Int_t&    chrg() const { return (info_.chrg()); }
        inline const Double_t& mass() const { return (info_.mass()); }

        inline const Double_t& mom()   const { return mom_; }
        inline const Double_t& eng()   const { return eng_; } 
        inline const Double_t& bta()   const { return bta_; }
        inline const Double_t& gmbta() const { return gmbta_; }
        inline const Double_t& eta()   const { return eta_; }
      
        inline Double_t gm() const { return ((MGNumc::EqualToZero(bta_)) ? MGMath::ONE : (gmbta_/bta_)); }

        inline Short_t  eta_sign() const { return (MGNumc::Compare(eta_)); }
        inline Double_t eta_abs()  const { return std::fabs(eta_); }

        inline const Double_t& irig()  const { return irig_; }
        inline Double_t rig() const { return (MGNumc::EqualToZero(irig_) ? MGMath::ZERO : MGMath::ONE / irig_); }

        inline const SVecD<3>& c()  const { return coo_; } 
        inline const Double_t& cx() const { return coo_(0); } 
        inline const Double_t& cy() const { return coo_(1); } 
        inline const Double_t& cz() const { return coo_(2); } 
        
        inline const SVecD<3>& u()  const { return dir_; }
        inline const Double_t& ux() const { return dir_(0); } 
        inline const Double_t& uy() const { return dir_(1); } 
        inline const Double_t& uz() const { return dir_(2); } 
        
        inline Double_t tx() const { return ((MGNumc::EqualToZero(dir_(2))) ? 0. : dir_(0)/dir_(2)); } 
        inline Double_t ty() const { return ((MGNumc::EqualToZero(dir_(2))) ? 0. : dir_(1)/dir_(2)); } 

        inline void set_st(const SVecD<5>& st) { set_state_with_uxy(SVecD<3>(st(0), st(1), coo_(2)), SVecD<3>(st(2), st(3), MGNumc::Compare(dir_(2)))); set_eta(st(4)); }
        inline SVecD<5> st() const { return SVecD<5>(coo_(0), coo_(1), dir_(0), dir_(1), eta_); }

        inline PhyArg&       arg() { return arg_; }
        inline VirtualPhySt& vst() { return vst_; }
        
        inline const Bool_t& field() const { return arg_(); }

        void zero() { arg_.reset(arg_.mscat(), arg_.eloss()); vst_.reset(); }
        void symbk(Bool_t is_rndm = false);

    private :
        PartInfo info_;

        Double_t mom_;
        Double_t eng_;
        Double_t bta_;
        
        Double_t gmbta_;
        Double_t eta_;
        
        Double_t irig_;
        
        SVecD<3> coo_;
        SVecD<3> dir_;

        PhyArg       arg_;
        VirtualPhySt vst_;
};
        

} // namespace TrackSys


#endif // __TRACKLibs_PhySt_H__
