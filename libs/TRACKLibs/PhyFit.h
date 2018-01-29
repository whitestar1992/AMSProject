#ifndef __TRACKLibs_PhyFit_H__
#define __TRACKLibs_PhyFit_H__

#include "Minuit2/Minuit2Minimizer.h"

namespace TrackSys {


class TrFitPar {
    public :
        enum class Orientation {
            kDownward = 0, kUpward = 1
        };

    public :
        TrFitPar(const PartType& type = PartType::Proton, const Orientation& ortt = Orientation::kDownward, Bool_t sw_mscat = PhyArg::OptMscat(), Bool_t sw_eloss = PhyArg::OptEloss());
        ~TrFitPar() { clear(); }
   
        inline void addHit(HitSt& hit) { hits_.push_back(hit); }
        inline Short_t numOfHit() const { return hits_.size(); }
    
    protected :
        Bool_t checkHit();
        void clear();

    protected :
        Bool_t      sw_mscat_;
        Bool_t      sw_eloss_;
        PartType    type_;
        Orientation ortt_;

        std::vector<HitSt> hits_;
        Short_t            nhtx_;
        Short_t            nhty_;

    protected :
        // Number of Hit Requirement
        static constexpr Short_t LMTL_NHIT_X = 3;
        static constexpr Short_t LMTL_NHIT_Y = 4;
};


class SimpleTrFit : public TrFitPar {
    public :
        SimpleTrFit(TrFitPar& fitPar); 
        ~SimpleTrFit() { SimpleTrFit::clear(); TrFitPar::clear(); }
        
    public :
        inline const Bool_t& status() { return succ_; }
        inline const PhySt& part() const { return part_; }
        
        inline const Double_t& nchi() const { return nchi_; }
        inline Double_t nchix() const { return succ_?(chix_/ndfx_):0; }
        inline Double_t nchiy() const { return succ_?(chiy_/ndfy_):0; }

    protected :
        void clear();

        Bool_t analyticalFit();
        Bool_t simpleFit();

    protected :
        Bool_t   succ_;
        PhySt    part_;
        
        Double_t ndfx_;
        Double_t ndfy_;
        Double_t chix_;
        Double_t chiy_;

        Double_t ndof_;
        Double_t nchi_;

    protected :
        // Minimization (Levenberg-Marquardt Method)
        static constexpr Int_t    LMTL_ITER = 3;
        static constexpr Int_t    LMTU_ITER = 25;
        
        static constexpr Double_t LAMBDA0 = 1.0e-2;
        static constexpr Double_t LAMBDA_DN_FAC = 5.0;
        static constexpr Double_t LAMBDA_UP_FAC = 7.0;
        static constexpr Double_t LMTL_LAMBDA = 1.0e-4;
        static constexpr Double_t LMTU_LAMBDA = 1.0e+3;
        
        static constexpr Double_t CONVG_EPSILON   = 3.0e-3;
        static constexpr Double_t CONVG_TOLERANCE = 7.0e-3;
};



//#include "Math/Minimizer.h"
//#include "Math/Factory.h"
//#include "Math/Functor.h"
//class VirtualPhyTrFit : public TrFitPar, virtual public ROOT::Math::IGradientFunctionMultiDim {
//    public :
//        VirtualPhyTrFit(TrFitPar& fitPar) {} 
//        ~VirtualPhyTrFit() { TrFitPar::clear(); }
//
//        void FdF(const double *x, double &f, double *df) {}
//
//    protected :
//        void set(const double *x) {}
//
//    protected :
//        PhySt               part_;
//        std::vector<PhyArg> args_;
//};


class PhyTrFit : public TrFitPar {
    public :
        PhyTrFit(TrFitPar& fitPar) {}
        ~PhyTrFit() {}
};











class PhyTr {
    public :
        enum class Orientation {
            kDownward = 0, kUpward = 1
        };

        inline static Bool_t HitCheck(const std::vector<HitSt>& hits);
        inline static std::tuple<Short_t, std::vector<Short_t>, std::vector<Short_t>, std::vector<Short_t>, Short_t, Short_t> HitSort(std::vector<HitSt>& hits, Orientation ortt = Orientation::kDownward);
        
    public :
        PhyTr(const std::vector<HitSt>& hits, const PartType& type = PartType::Proton, const Orientation& ortt = Orientation::kDownward, Bool_t sw_mscat = PhyArg::OptMscat(), Bool_t sw_eloss = PhyArg::OptEloss());
        ~PhyTr() { clear(); }

        void print() const;

        Bool_t fit();

        inline const PhySt& part() const { return part_; }
        inline const Double_t& nchi() const { return nchi_; }
        inline Double_t nchix() const { return (chix_/ndfx_); }
        inline Double_t nchiy() const { return (chiy_/ndfy_); }

    protected :
        void clear();
        
        // Fitting
        Bool_t fit_physics();
        
    private :
        Bool_t sw_mscat_;
        Bool_t sw_eloss_;
        
        PartType             type_;
        Orientation          ortt_;
        Short_t              nseq_; // tot seq
        std::vector<Short_t> seqx_; // x -> seq
        std::vector<Short_t> seqy_; // y -> seq
        std::vector<Short_t> maps_; // seq -> (x=0 or y=1) * nhit + hitID
        std::vector<HitSt>   hits_;
        Short_t              nhtx_;
        Short_t              nhty_;

        // Fitting
        Bool_t   succ_;
        PhySt    part_;
        Double_t ndfx_;
        Double_t ndfy_;
        Double_t chix_;
        Double_t chiy_;
        Double_t nchi_;

    protected :
        // Dimension
        static constexpr Int_t DIM_G = 5;
        static constexpr Int_t DIM_L = 4;

        // Minimization (Levenberg-Marquardt Method)
        static constexpr Int_t    LMTL_ITER = 3;
        static constexpr Int_t    LMTU_ITER = 30;
        
        static constexpr Double_t LAMBDA0 = 1.0e-2;
        static constexpr Double_t LAMBDA_DN_FAC = 7.0;
        static constexpr Double_t LAMBDA_UP_FAC = 9.0;
        static constexpr Double_t LMTL_LAMBDA = 1.0e-4;
        static constexpr Double_t LMTU_LAMBDA = 1.0e+3;
        
        static constexpr Double_t CONVG_EPSILON   = 2.0e-3;
        static constexpr Double_t CONVG_TOLERANCE = 5.0e-3;
};


} // namespace TrackSys


#endif // __TRACKLibs_PhyFit_H__