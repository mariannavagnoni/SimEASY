#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1


#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "Levels.hh"

#include "G4ParticleGun.hh"

//modificato
//--------------------
#include "G4String.hh"
#include "G4Gamma.hh"
#include "G4Positron.hh"
#include "G4Event.hh"
#include "G4RandomDirection.hh"
#include "Randomize.hh"
#include "CLHEP/Random/RandGeneral.h"

//---------------------
#include "DataBin.hh"
//#include "globals.hh"
#include <memory>

using std::shared_ptr;
using std::make_shared;

class G4Event;
class G4ParticleGun;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction, public G4UImessenger
{
    public:
        PrimaryGeneratorAction(levelvec* l, DataBin* d);
        virtual ~PrimaryGeneratorAction();

        void GeneratePrimaries(G4Event* );
        void GeneratePrimaryGamma(G4int, G4ThreeVector, G4double Eg, G4Event*);

        //void GenerateSecondaryGamma(unsigned long j,  G4ThreeVector, G4double Eg, G4Event*);
        const G4double* getDaughterEnergy() const {return Ed;}

    private:
        G4ThreeVector m_center = G4ThreeVector(0,0,0);
        G4double m_length = 22.; //*CLHEP::cm;
        G4double m_radius = 1.1; //*CLHEP::cm;
        G4ThreeVector p1;
        levelvec* levels; 
        G4double firstExcitedEnergy;
        DataBin* databin;
        G4double* Ed;
        //G4ParticleDefinition *m_gammaDefinition = nullptr;

        G4ParticleGun* particleGun;

        G4int GenerateUniqueID();
    
};

#endif // #ifndef PrimaryGeneratorAction_hh
