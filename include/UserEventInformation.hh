#ifndef UserEventInformation_hh
#define UserEventInformation_hh

#include "G4VUserEventInformation.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

struct ParticleInfo {
    G4double energy;
    G4ThreeVector position;
    G4ThreeVector direction;
    G4int id;
};

class UserEventInformation : public G4VUserEventInformation {
    public:
    UserEventInformation() {};
        virtual ~UserEventInformation() {};

        void AddParticle(const ParticleInfo& info){
            particles.push_back(info);
        }

        const std::vector<ParticleInfo>& GetParticles() const {
            return particles;
        }


        virtual void Print() const {};

    private:
        std::vector<ParticleInfo> particles;
};


#endif
