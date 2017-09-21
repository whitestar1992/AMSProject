#ifndef __TRACKLibs_HitSt_H__
#define __TRACKLibs_HitSt_H__


namespace TrackSys {


class HitSt {
    public :
        enum class Orientation {
            kDownward = 0, kUpward = 1
        };
    
    public :
        HitSt(Int_t id = -1, Bool_t sx = true, Bool_t sy = true, Bool_t sz = true) : id_(id), side_(sx, sy, sz), coo_(0., 0., 0.), err_x_(DEFAULT_ERR_X_), err_y_(DEFAULT_ERR_Y_), err_z_(DEFAULT_ERR_Z_) {}
        ~HitSt() {}

        inline void set_id(Int_t id) { id_ = id; }
        inline void set_coo(Double_t cx, Double_t cy, Double_t cz) { coo_(0) = cx, coo_(1) = cy, coo_(2) = cz; }
        inline void set_err(const MultiGauss& ex = DEFAULT_ERR_X_, const MultiGauss& ey = DEFAULT_ERR_Y_, const MultiGauss& ez = DEFAULT_ERR_Z_) { err_x_ = ex; err_y_ = ey; err_z_ = ez; }
        inline void set_coo_and_err(Double_t cx, Double_t cy, Double_t cz, const MultiGauss& ex = DEFAULT_ERR_X_, const MultiGauss& ey = DEFAULT_ERR_Y_, const MultiGauss& ez = DEFAULT_ERR_Z_) { set_coo(cx, cy, cz); set_err(ex, ey, ez); }

        inline void set_dummy_x(Double_t cx) { coo_(0) = cx; }

        void print() const;

        inline const Int_t& id() const { return id_; }
        
        inline const SVecO<3>& side() const { return side_; }
        inline const SVecD<3>& coo() const { return coo_; }

        inline SVecD<3> err(const SVecD<3>& res = SVecD<3>()) const { return SVecD<3>(err_x_.efft_sgm(res(0)), err_y_.efft_sgm(res(1)), err_z_.efft_sgm(res(2))); }
        inline SVecD<2> err(const SVecD<2>& res) const { return SVecD<2>(err_x_.efft_sgm(res(0)), err_y_.efft_sgm(res(1))); }
        
        inline const Bool_t&   sx() const { return side_(0); }
        inline const Bool_t&   sy() const { return side_(1); }
        inline const Bool_t&   sz() const { return side_(2); }

        inline const Double_t& cx() const { return coo_(0); }
        inline const Double_t& cy() const { return coo_(1); }
        inline const Double_t& cz() const { return coo_(2); }
        
        inline const MultiGauss& ex() const { return err_x_; }
        inline const MultiGauss& ey() const { return err_y_; }
        inline const MultiGauss& ez() const { return err_z_; }

        inline Double_t ex(Double_t r) const { return err_x_.efft_sgm(r); }
        inline Double_t ey(Double_t r) const { return err_y_.efft_sgm(r); }
        inline Double_t ez(Double_t r) const { return err_z_.efft_sgm(r); }

    private :
        Int_t      id_;
        SVecO<3>   side_;
        SVecD<3>   coo_;     // [cm]
        MultiGauss err_x_;   // [cm]
        MultiGauss err_y_;   // [cm]
        MultiGauss err_z_;   // [cm]
    
    public :
        static void SetDefaultErrX(const MultiGauss& ex = DEFAULT_ERR_X_) { DEFAULT_ERR_X_ = ex; }
        static void SetDefaultErrY(const MultiGauss& ey = DEFAULT_ERR_Y_) { DEFAULT_ERR_Y_ = ey; }
        static void SetDefaultErrZ(const MultiGauss& ez = DEFAULT_ERR_Z_) { DEFAULT_ERR_Z_ = ez; }

        static const MultiGauss& GetDefaultErrX() { return DEFAULT_ERR_X_; }
        static const MultiGauss& GetDefaultErrY() { return DEFAULT_ERR_Y_; }
        static const MultiGauss& GetDefaultErrZ() { return DEFAULT_ERR_Z_; }
        
        static Double_t GetDefaultErrX(Double_t r) { return DEFAULT_ERR_X_.efft_sgm(r); }
        static Double_t GetDefaultErrY(Double_t r) { return DEFAULT_ERR_Y_.efft_sgm(r); }
        static Double_t GetDefaultErrZ(Double_t r) { return DEFAULT_ERR_Z_.efft_sgm(r); }

    protected :
        static MultiGauss DEFAULT_ERR_X_;
        static MultiGauss DEFAULT_ERR_Y_;
        static MultiGauss DEFAULT_ERR_Z_;
        
    public :
        static void Sort(std::vector<HitSt>& hits, const Orientation& ortt = Orientation::kDownward) {
            if (hits.size() < 2) return;
            if (ortt == Orientation::kDownward) std::sort(hits.begin(), hits.end(), [](const HitSt& hit1, const HitSt& hit2) { return (hit1.cz() > hit2.cz()); } );
            else                                std::sort(hits.begin(), hits.end(), [](const HitSt& hit1, const HitSt& hit2) { return (hit1.cz() < hit2.cz()); } );
        }
};
        
MultiGauss HitSt::DEFAULT_ERR_X_(24.0e-4);
MultiGauss HitSt::DEFAULT_ERR_Y_(10.0e-4);
MultiGauss HitSt::DEFAULT_ERR_Z_(300.0e-4);


} // namesapce TrackSys


#endif // __TRACKLibs_HitSt_H__
