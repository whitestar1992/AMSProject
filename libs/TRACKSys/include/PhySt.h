#ifndef __TRACKLibs_PhySt_H__
#define __TRACKLibs_PhySt_H__


namespace TrackSys {


class PhyArg {
    public :
        static void SetOpt(Bool_t opt_mscat = false, Bool_t opt_eloss = false) { opt_mscat_ = opt_mscat; opt_eloss_ = opt_eloss; }
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

        inline void zero();
        inline void reset(Bool_t sw_mscat = OptMscat(), Bool_t sw_eloss = OptEloss());
        inline void clear() { reset(sw_mscat_, sw_eloss_); }

        inline Bool_t operator() () const { return (field_ && mat_); }

        inline const Bool_t& mscat() const { return sw_mscat_; }
        inline const Bool_t& eloss() const { return sw_eloss_; }

        inline const Bool_t& field() const { return field_; }
        inline const Bool_t& mat() const { return mat_; }

        inline const Double_t& len() const { return len_; }
        inline const Double_t& nrl() const { return nrl_; }
        inline const Double_t& ela() const { return ela_; }

        inline const Double_t& tauu() const { return tauu_; }
        inline const Double_t& rhou() const { return rhou_; }
        inline const Double_t& taul() const { return taul_; }
        inline const Double_t& rhol() const { return rhol_; }
        
        inline const Double_t& elion() const { return elion_; }
        inline const Double_t& elbrm() const { return elbrm_; }
       
        inline Double_t etauu() const { return pdf_mscatu_.efft_sgm(tauu_); }
        inline Double_t erhou() const { return pdf_mscatu_.efft_sgm(rhou_); }
        inline Double_t etaul() const { return pdf_mscatl_.efft_sgm(taul_); }
        inline Double_t erhol() const { return pdf_mscatl_.efft_sgm(rhol_); }
        
        inline Double_t eelion() const { return pdf_elion_.efft_sgm(elion_); }
  
        inline const Short_t&  sign() const { return sign_; }
        inline const SVecD<3>& orth_tau() const { return orth_tau_; }
        inline const SVecD<3>& orth_rho() const { return orth_rho_; }
        inline const Double_t& orth_tau(Int_t i) const { return orth_tau_(i); }
        inline const Double_t& orth_rho(Int_t i) const { return orth_rho_(i); }
        
        inline const Double_t& mscat_uu() const { return mscat_uu_; }
        inline const Double_t& mscat_ul() const { return mscat_ul_; }
        inline const Double_t& mscat_ll() const { return mscat_ll_; }

        inline const Double_t& elion_mpv() const { return elion_mpv_; }
        inline const Double_t& elion_sgm() const { return elion_sgm_; }
        inline const Double_t& elbrm_men() const { return elbrm_men_; }

    public :
        // Set Parameters
        void set_mscat(Double_t tauu = 0, Double_t rhou = 0, Double_t taul = 0, Double_t rhol = 0) { if (sw_mscat_) { tauu_ = tauu; rhou_ = rhou; taul_ = taul; rhol_ = rhol; } }
        void set_eloss(Double_t ion = 0, Double_t brm = 0) { if (sw_eloss_) { elion_ = ion; elbrm_ = ((brm<0.0)?0.0:brm); } }

        // Rndm Parameters
        void rndm_mscatu() { tauu_ = 0.; rhou_ = 0.; if (sw_mscat_) { tauu_ = pdf_mscatu_.rndm(); rhou_ = pdf_mscatu_.rndm(); } }
        void rndm_mscatl() { taul_ = 0.; rhol_ = 0.; if (sw_mscat_) { taul_ = pdf_mscatl_.rndm(); rhol_ = pdf_mscatl_.rndm(); } }
        void rndm_mscat() { rndm_mscatu(); rndm_mscatl(); }

        //void rndm_elion() { elion_ = 0.; if (sw_eloss_) { elion_ = Rndm::Landau(); } }
        void rndm_elion() { elion_ = 0.; if (sw_eloss_) { elion_ = Rndm::NormalGaussian(); } } // testcode
        void rndm_elbrm() { elbrm_ = 0.; if (sw_eloss_) { Double_t bremslen = nrl_ / Numc::LOG_TWO<>; elbrm_ = ((nrl_<=0.0)?0.0:Rndm::Gamma(bremslen,1.0/bremslen)()); } }
        void rndm_eloss() { rndm_elion(); rndm_elbrm(); }

        void rndm() { if (field_) { rndm_mscatu(); rndm_mscatl(); rndm_elion(); rndm_elbrm(); } }

        // Set Variables
        void setvar_len(Double_t len = 0) { len_ = ((len>=0.)?len:0.); }
        void setvar_mat(Bool_t mat = false, Double_t nrl = 0, Double_t ela = 0) { if (mat) { mat_ = mat; nrl_ = nrl; ela_ = ela; } }
        void setvar_orth(Short_t sign = 1, const SVecD<3>& tau = SVecD<3>(1, 0, 0), const SVecD<3>& rho = SVecD<3>(0, 1, 0)) { sign_=((sign>=0)?1:-1); orth_tau_ = tau; orth_rho_ = rho; }
        void setvar_mscat(Double_t mscat_uu = 0, Double_t mscat_ul = 0, Double_t mscat_ll = 0) { if (sw_mscat_) { mscat_uu_ = mscat_uu; mscat_ul_ = mscat_ul; mscat_ll_ = mscat_ll; } }
        void setvar_eloss(Double_t elion_mpv = 0, Double_t elion_sgm = 0, Double_t elbrm_men = 0) { if (sw_eloss_) { elion_mpv_ = elion_mpv; elion_sgm_ = elion_sgm; elbrm_men_ = elbrm_men; } }
        
        // Symbk
        SVecD<3> symbk_mscatu() const { return ((sw_mscat_ && mat_) ? ((tauu_*mscat_uu_) * orth_tau_ + (rhou_*mscat_uu_) * orth_rho_) : SVecD<3>()); }
        SVecD<3> symbk_mscatl() const { return ((sw_mscat_ && mat_) ? ((tauu_*mscat_ul_ + taul_*mscat_ll_) * orth_tau_ + (rhou_*mscat_ul_ + rhol_*mscat_ll_) * orth_rho_) : SVecD<3>()); }
        Double_t symbk_eloss() const  { return ((sw_eloss_ && mat_) ? (sign_ * (elion_*elion_sgm_ + elbrm_*elbrm_men_)) : Numc::ZERO<>); }

    private :
        Bool_t   sw_mscat_;
        Bool_t   sw_eloss_;
        
        Bool_t   field_;
        Bool_t   mat_;
        Double_t len_;
        Double_t nrl_;
        Double_t ela_;

        Double_t tauu_;
        Double_t rhou_;
        Double_t taul_;
        Double_t rhol_;
        Double_t elion_;
        Double_t elbrm_;
        
        Short_t  sign_;      // sign(step)
        SVecD<3> orth_tau_;  // default (1, 0, 0)
        SVecD<3> orth_rho_;  // default (0, 1, 0)

        Double_t mscat_uu_;
        Double_t mscat_ul_;
        Double_t mscat_ll_;

        Double_t elion_mpv_;
        Double_t elion_sgm_;
        Double_t elbrm_men_;

    protected :
        static MultiGauss pdf_mscatu_;
        static MultiGauss pdf_mscatl_;
        static MultiGauss pdf_elion_;
};

Bool_t PhyArg::opt_mscat_ = false;
Bool_t PhyArg::opt_eloss_ = false;

MultiGauss PhyArg::pdf_mscatu_(
    MultiGauss::Opt::NOROBUST,
    7.99549384311964539e-01, 1.000000e+00,
    1.78119391133381794e-01, 1.579653e+00,
    1.94920516860290043e-02, 3.418158e+00,
    2.83917286862460675e-03, 9.314250e+00
);

MultiGauss PhyArg::pdf_mscatl_(
    MultiGauss::Opt::NOROBUST, 1.0
);

MultiGauss PhyArg::pdf_elion_(
    MultiGauss::Opt::NOROBUST, 1.0
);


class PhySt {
    public :
        PhySt(const PartType& type = PartType::Proton, Bool_t sw_mscat = PhyArg::OptMscat(), Bool_t sw_eloss = PhyArg::OptEloss()) : arg_(sw_mscat, sw_eloss) { reset(type); }
        PhySt(const PartInfo& info, Bool_t sw_mscat = PhyArg::OptMscat(), Bool_t sw_eloss = PhyArg::OptEloss()) : arg_(sw_mscat, sw_eloss) { reset(info.type()); }
        ~PhySt() {}

        void reset(const PartType& type = PartType::Proton);
        
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

    public :
        inline PhyArg&       arg() { return arg_; }
        inline const Bool_t& field() const { return arg_.field(); }
        inline const Bool_t& mat()   const { return arg_.mat(); }

        inline const PartInfo& info() const { return info_; }
        inline const Short_t&  chrg() const { return (info_.chrg()); }
        inline const Double_t& mass() const { return (info_.mass()); }

        inline const Double_t& mom()   const { return mom_; }
        inline const Double_t& eng()   const { return eng_; } 
        inline const Double_t& ke()    const { return ke_; }
        inline const Double_t& bta()   const { return bta_; }
        inline const Double_t& gmbta() const { return gmbta_; }
        inline const Double_t& eta()   const { return eta_; }
      
        inline Double_t gm() const { return ((Numc::EqualToZero(bta_)) ? Numc::ONE<> : (gmbta_/bta_)); }

        inline Short_t  eta_sign() const { return (Numc::Compare(eta_)); }
        inline Double_t eta_abs()  const { return std::fabs(eta_); }

        inline const Double_t& irig()  const { return irig_; }
        inline Double_t rig() const { return (Numc::EqualToZero(irig_) ? Numc::ZERO<> : Numc::ONE<> / irig_); }

        inline const SVecD<3>& c()  const { return coo_; } 
        inline const Double_t& cx() const { return coo_(0); } 
        inline const Double_t& cy() const { return coo_(1); } 
        inline const Double_t& cz() const { return coo_(2); } 
        
        inline const SVecD<3>& u()  const { return dir_; }
        inline const Double_t& ux() const { return dir_(0); } 
        inline const Double_t& uy() const { return dir_(1); } 
        inline const Double_t& uz() const { return dir_(2); } 
        inline Double_t        tx() const { return ((Numc::EqualToZero(dir_(2))) ? 0. : dir_(0)/dir_(2)); } 
        inline Double_t        ty() const { return ((Numc::EqualToZero(dir_(2))) ? 0. : dir_(1)/dir_(2)); } 

        void symbk(Bool_t is_rndm = false);

    private :
        PartInfo info_;
        PhyArg   arg_;

        Double_t mom_;
        Double_t eng_;
        Double_t ke_;
        Double_t bta_;
        Double_t gmbta_;
        Double_t eta_;
        
        Double_t irig_;
        
        SVecD<3> coo_;
        SVecD<3> dir_;
};
        

} // namespace TrackSys


#endif // __TRACKLibs_PhySt_H__
