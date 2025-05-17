#ifndef UserEventInformation_hh
#define UserEventInformation_hh

#include "G4VUserEventInformation.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"


class UserEventInformation : public G4VUserEventInformation {
    public:
    UserEventInformation(G4double E, const G4ThreeVector &pos, const G4ThreeVector &dir) :
            m_E(E), m_pos(pos), m_dir(dir) {};
        virtual ~UserEventInformation() {};

        G4double GetE() const {return m_E;}
        const G4ThreeVector &GetPosition() const {return m_pos;}
        G4double GetPhi() const {return m_dir.phi();}
        G4double GetCosTheta() const {return m_dir.cosTheta();}

        virtual void Print() const {};

    private:
        const G4double m_E = 0;
        const G4ThreeVector m_pos;
        const G4ThreeVector m_dir;
};


#endif
