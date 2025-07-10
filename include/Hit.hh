
#ifndef Hit_h
#define Hit_h 1

#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"
#include "G4VHit.hh"

//The scintillator hit class is derived from G4VHit. It defines data members to store the energy deposit and track lengths of charged particles in a selected volume.

class MyHit : public G4VHit
{
public:
    MyHit();
    virtual ~MyHit();
    
    virtual void Draw(){}
    virtual void Print(){}
    
    //Set method
    inline void SetEdep(G4double energy){energyDeposit = energy;}
    inline void SetTime(G4double time){timeDeposit = time;}
    G4double GetTrackLength() const;
    inline void SetID(G4int cN){ID = cN;}
   
    //Get method
    inline G4double GetEdep(){return energyDeposit;}
    inline G4double GetTime(){return timeDeposit;}
    inline G4int GetID(){return ID;}
      
  private:
    G4double energyDeposit;// energy deposited in the sensitive volume
    G4double timeDeposit;// time the energy is deposited in the sensitive volume 
    G4int ID;//ID of the detector
    
    G4double fTrackLength; ///< Track length in the  sensitive volume
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



#include "G4THitsCollection.hh"
typedef G4THitsCollection<MyHit> MyHitsCollection;


#endif
