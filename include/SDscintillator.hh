#ifndef SDscintillator_h
#define SDscintillator_h 1

#include "G4VSensitiveDetector.hh"
#include "Hit.hh"
class G4Step;
class G4HcofThisEvent;

class MySensitiveDetector : public G4VSensitiveDetector
{
	public:
		MySensitiveDetector(G4String SDname, const G4String& hitsCollectionName);
		virtual ~MySensitiveDetector();
        virtual void Initialize(G4HCofThisEvent* hitCollection);
        virtual void EndOfEvent(G4HCofThisEvent* hitCollection);
        virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);
    private:
    MyHitsCollection* hitsCollection;
    G4int collectionID= -1;
};


#endif
