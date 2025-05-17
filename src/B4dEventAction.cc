//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B4dEventAction.cc
/// \brief Implementation of the B4dEventAction class

#include "B4dEventAction.hh"
#include "B4Analysis.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "Hit.hh"
#include "G4UnitsTable.hh"
#include "UserEventInformation.hh"
#include "SDscintillator.hh"
#include "Randomize.hh"
#include <iomanip>

using CLHEP::MeV;
using CLHEP::m;

    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    
B4dEventAction::B4dEventAction() : G4UserEventAction()
{
    G4RunManager* fRM= G4RunManager::GetRunManager();
    const G4Event* event = fRM-> GetCurrentEvent();
}
    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
    
B4dEventAction::~B4dEventAction(){}
    
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    

void B4dEventAction::BeginOfEventAction(const G4Event*  /*event*/){}
    
    
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    
void B4dEventAction::EndOfEventAction(const G4Event* event)
{
    auto userInformation = static_cast<UserEventInformation*>(event->GetUserInformation());
    
    const double E        = userInformation->GetE();
    const double posX     = userInformation->GetPosition().x();
    const double posY     = userInformation->GetPosition().y();
    const double posZ     = userInformation->GetPosition().z();
    
    const double phi      = userInformation->GetPhi();
    const double cosTheta = userInformation->GetCosTheta();
    
    
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    
    analysisManager->FillNtupleIColumn(0, 0, event->GetEventID());
    analysisManager->FillNtupleDColumn(0, 1, E / MeV);
    analysisManager->FillNtupleDColumn(0, 2, posX / m);
    analysisManager->FillNtupleDColumn(0, 3, posY / m);
    analysisManager->FillNtupleDColumn(0, 4, posZ / m);
    analysisManager->FillNtupleDColumn(0, 5, phi);
    analysisManager->FillNtupleDColumn(0, 6, cosTheta);
    analysisManager->AddNtupleRow(0);
    
    //std::vector<G4double*> energy;
    G4double energy[8] = {0};
    G4double Energy_tot=0.;
    G4int count = 0;
    G4int numberHits=0;
    
    // Get hist collections ID
    if (m_HCID == -1 )
    {
        G4SDManager* sdm = G4SDManager::GetSDMpointer();
        
        
        if (sdm->FindSensitiveDetector("scintDet") == nullptr)
        {
            G4cout << "ERROR: Did not find sensitive detector scintDet" << G4endl;
            m_HCID = -100;
        }
        else
        {
            m_HCID
            = sdm->GetCollectionID("scintDet/Edep");
        }
    }
    if (m_HCID >= 0)
    {
        G4HCofThisEvent* HCE = event->GetHCofThisEvent();
        
        //G4cout << "_____________" << std::endl;
        //G4cout << "Hit Collection ID "<< m_HCID << std::endl;
        
        
        if(HCE)
        {
            hitsColl = static_cast<MyHitsCollection*>(HCE->GetHC(m_HCID));
            
            
            if(hitsColl)
            {
                
                
                numberHits=hitsColl->entries();
            
                
                for(G4int j=0;j<numberHits; j++)
                {
                    MyHit* hit = (*hitsColl)[j];
                    G4double de = hit->GetEdep();
                    Energy_tot += de;
                    energy[hit->GetID()] += de;
                    
                }
                
                for(G4int i=1; i < 8; i++)
                    if(energy[i]>0) analysisManager->FillH1(i, energy[i] / MeV); //Fill value as MeV
                
                if(Energy_tot>0){
                    
                    analysisManager->FillH1(0, Energy_tot / MeV); //Fill value as MeV
                    // Fill energy deposition
                    analysisManager->FillNtupleIColumn(1, 0, event->GetEventID());
                    //detector ID Copy number
                    analysisManager->FillNtupleDColumn(1, 1, Energy_tot/MeV);
                    analysisManager->FillNtupleDColumn(1, 2, E / MeV);
                    analysisManager->FillNtupleDColumn(1, 3, posX / m);
                    analysisManager->FillNtupleDColumn(1, 4, posY / m);
                    analysisManager->FillNtupleDColumn(1, 5, posZ / m);
                    analysisManager->FillNtupleDColumn(1, 6, phi);
                    analysisManager->FillNtupleDColumn(1, 7, cosTheta);
                    //analysisManager->FillNtupleDColumn(1, 8, Energy_tot);
                    analysisManager->AddNtupleRow(1);
                    
                }
                
                G4double sum=0;
                for(G4int i=1; i < 8; i++){
                    sum+=energy[i];
                }
                
                analysisManager->FillH1(8, sum / MeV);
                
                if(Energy_tot==0) count++;
                //G4cout << "number of step " << numberHits << std::endl;
                //G4cout << "number of step with total 0 deposited energy" << count << std::endl;
            }
        else
            {
                G4cout << "hitsColl does not exist" << std::endl;
                
            }
            
            
            
            
           
                
                //for(G4int i=0; i<7; i++)
                /*
                 Energy vector<double>
                 
                 
                 G4int i = 0:
                 vector<int> vec = {1,2,3,4,5} //energia
                 
                 for(auto itr : vec){
                 histograms[i]->Fill(itr);
                 i++;
                 }
                 */
                
                // getchar();
            }
        else
            {
                
                G4cout << "HCE does not exist" << std::endl;
            }
            
           
        }
        
}

/*
            
            if (HCE) {
               
                //MyHitsCollection* EvtHC = (MyHitsCollection*)(HCE->GetHC(m_HCID));
                auto EvtHC =static_cast<G4THitsMap<G4double>*>(HCE->GetHC(m_HCID));
               
                //G4cout << EvtHC << std::endl;
                
                if (EvtHC) {
                    
                    //if(!((EvtHC->GetMap())->empty())){
                        
                        
                        for ( auto it : *EvtHC->GetMap() )
                            
                        {
                            analysisManager->FillH1(0, *(it.second) / MeV); //Fill value as MeV
                            
                            
                            
                            //G4int copyNo =
                            //G4cout << "_____" << copyNo <<std::endl;
                            // Fill energy deposition
                            analysisManager->FillNtupleIColumn(1, 0, event->GetEventID());
                            //detector ID Copy number
                            analysisManager->FillNtupleIColumn(1, 1, it.first);
                            analysisManager->FillNtupleDColumn(1, 2, E / MeV);
                            analysisManager->FillNtupleDColumn(1, 3, posX / m);
                            analysisManager->FillNtupleDColumn(1, 4, posY / m);
                            analysisManager->FillNtupleDColumn(1, 5, posZ / m);
                            analysisManager->FillNtupleDColumn(1, 6, phi);
                            analysisManager->FillNtupleDColumn(1, 7, cosTheta);
                           // analysisManager->FillNtupleDColumn(1, 8, *(it.second) / MeV);
                            analysisManager->AddNtupleRow(1);
                            //G4cout <<"-------" << *(it.second) << std::endl;
                        }
                        //hitsCollection += *EvtHC;
                        //m_HCID++;
                        
                   // }
                    /*else
                    {
                        
                        std::cout << "Hit collection map is empty " << std::endl;
                    }*/
                    
                  /*
                    
                } else {
                   
            
                    G4cerr << "Failed to get hitsCollection ID" << G4endl;
                    exit(99);
                }
                   
               
            }*/
            
            
           
        
        
        
        //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    
    

