
#include "Hit.hh"
#include "SDscintillator.hh"
#include "G4Step.hh"
#include "G4TouchableHandle.hh"
#include "G4SDManager.hh"


MySensitiveDetector::MySensitiveDetector(const G4String name,const G4String& hitsCollectionName)
: G4VSensitiveDetector(name)
{
    collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MySensitiveDetector::~MySensitiveDetector()
{
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MySensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
    
    //if (collectionID<0) collectionID=GetCollectionID(0);
        G4int collectionID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    
    G4cout << "collectionID" << collectionID << std::endl;
    
    
    
        hitsCollection = new MyHitsCollection(SensitiveDetectorName, collectionName[0]);
        
        
        
        hce->AddHitsCollection(collectionID, hitsCollection);
    
}

G4bool MySensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *ROhist)
{
    
   
    //Accessing the current step

    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4StepPoint* postStepPoint = step ->GetPostStepPoint();
    G4ThreeVector pos1 = preStepPoint->GetPosition();
    G4ThreeVector pos2 = postStepPoint->GetPosition();

    //G4cout << "initial position: " << pos1 << G4endl;
    //G4cout << "final position: " << pos2 << G4endl;
    //G4cout << "ooOOOooo" << std::endl;
    
    G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();
    G4int copyNo = theTouchable->GetCopyNumber();
    //G4cout << "Copy number of the scint volume: " << copyNo << G4endl;
    //G4int motherCopyNo = theTouchable->GetVolume(1)->GetCopyNo();
    //G4cout << "Copy number of the vacuum volume: " << motherCopyNo << G4endl;
    G4int grandMotherCopyNo = theTouchable->GetCopyNumber(2);
    //G4cout << "Copy number of the mother volume: " << grandMotherCopyNo << G4endl;
   
    
    G4double stepLength = step ->GetStepLength();
    //G4cout << "Step Length: " << stepLength /CLHEP::mm << G4endl;
    
    MyHit* hit = new MyHit();
    // Get some properties from G4Step and set them to the hit
    G4double energyDeposit = step->GetTotalEnergyDeposit();
    
    //G4cout << "Edep" << energyDeposit / CLHEP::keV << std::endl;
    hit->SetEdep(energyDeposit);
    hit->SetID(grandMotherCopyNo);
    hitsCollection->insert(hit);
    

    return true;

         
    
    
    
}

void MySensitiveDetector::EndOfEvent(G4HCofThisEvent* hce)
{
    
 }
