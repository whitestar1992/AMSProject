#ifndef __TRACKLibs_PartInfo_H__
#define __TRACKLibs_PartInfo_H__


namespace TrackSys {


enum class PartType {
    Photon, 
    Electron, Positron, 
    Muon, 
    PionPlus, PionMinus,
    KaonPlus, KaonMinus,
    Proton, AntiProton,
    Deuterium, AntiDeuterium,
    Helium3, AntiHelium3, Helium4, AntiHelium4,
    Lithium6, Lithium7,
    Beryllium7, Beryllium9, Beryllium10,
    Boron10, Boron11,
    Carbon12, Carbon13, Carbon14,
    Nitrogen13, Nitrogen14, Nitrogen15,
    Oxygen16, Oxygen17, Oxygen18
};


class PartInfo {
    public :
        PartInfo(const PartType& type = PartType::Proton);
        ~PartInfo() {}

        void print() const;

        inline const PartType&     type() const { return type_; }
        inline const std::string&  name() const { return name_; }
        inline const Short_t&      chrg() const { return chrg_; }
        inline const Double_t&     mass() const { return mass_; }

        inline const Bool_t& is_chrgless() const { return is_chrgless_; }
        inline const Bool_t& is_massless() const { return is_massless_; }
        
        inline const Double_t& mass_to_chrg() const { return mass_to_chrg_; }
        inline const Double_t& chrg_to_mass() const { return chrg_to_mass_; }


    private :
        PartType    type_;
        std::string name_;
        Short_t     chrg_;
        Double_t    mass_; // [GeV]
        
        Bool_t      is_chrgless_;
        Bool_t      is_massless_;
        
        Double_t    mass_to_chrg_;
        Double_t    chrg_to_mass_;
};


} // namespace TrackSys


#endif // __TRACKLibs_PartInfo_H__
