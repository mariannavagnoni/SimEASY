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
/// \file RunAction.cc
/// \brief Implementation of the RunAction class

#include "RunAction.hh"
#include "Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

RunAction::RunAction()
{
  //initialize the analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetDefaultFileType("root");

  // Default settings
  // Set to false to see .root files from individual threads
  // Note: merging ntuples is available only with Root output
  analysisManager->SetNtupleMerging(true); 
  analysisManager->SetFileName("defaultName");

}

RunAction::~RunAction()
{}

void RunAction::BeginOfRunAction(const G4Run* run){ //, G4double m_numEvents){

    auto analysisManager = G4AnalysisManager::Instance();

    //get the ID to distinguish between each run (useful for macros)
    //G4int runID = run->GetRunID()+1;
    //std::stringstream strRunID;
    //strRunID << runID;
    //std::stringstream evt ;
    //evt << m_numEvents;

    //Open a file
    if (analysisManager) {
      //analysisManager->OpenFile(m_fileName + evt.str()+".root");
      //analysisManager->OpenFile(m_fileName + strRunID.str()+".root");
      analysisManager->OpenFile();
    } 
    else {
      G4cerr << "Error: analysisManager is null in BeginOfRunAction!" << G4endl;
    }
    
    // This evaluates to false after the first time the file is opened in a run
    /*
    if(analysisManager->IsOpenFile()) {
        G4cout << "Analysis root file opened successfully!" << G4endl;
    } 
    else {
        G4cerr << "Error: Failed to open the analysis root file!" << G4endl;
    }
    */

    analysisManager->CreateH1("Tot_energy","Energy deposit",1400,0.*MeV,14*MeV); //

    for (G4int i = 1; i < 8; i++) {
        // Constructing the title dynamically
        G4String name = "Edeposit_" + std::to_string(i);
        G4String title = "Energy deposit for scintillator " + std::to_string(i);

        // Create the histogram with the dynamically constructed title
        analysisManager->CreateH1(name, title, 1400, 0.*MeV, 14.*MeV);
    }

    analysisManager->CreateH1("Sum_individuals","Sum energy deposit",1400,0.*MeV,14*MeV);

    //h1 - energy deposited

    //Ntuple to store Primary Gamma
    //analysisManager->CreateNtuple("Primary", "Primary");
    //Ntuple to store all emitted Gammas
    analysisManager->CreateNtuple("Emitted", "Emitted");
    //create column
    analysisManager->CreateNtupleIColumn("ID");
    analysisManager->CreateNtupleDColumn("E");
    analysisManager->CreateNtupleDColumn("x");
    analysisManager->CreateNtupleDColumn("y");
    analysisManager->CreateNtupleDColumn("z");
    analysisManager->CreateNtupleDColumn("phi");
    analysisManager->CreateNtupleDColumn("cosTheta");

    analysisManager->FinishNtuple(0);

    //Ntuple to store the deposited energy
    analysisManager->CreateNtuple("Edep", "Edep");
    //create Column
    analysisManager->CreateNtupleIColumn("ID");
    analysisManager->CreateNtupleDColumn("Edep");
    //analysisManager->CreateNtupleIColumn("detectorID");
    analysisManager->CreateNtupleDColumn("E");
    analysisManager->CreateNtupleDColumn("x");
    analysisManager->CreateNtupleDColumn("y");
    analysisManager->CreateNtupleDColumn("z");
    analysisManager->CreateNtupleDColumn("phi");
    analysisManager->CreateNtupleDColumn("cosTheta");
    //analysisManager->CreateNtupleDColumn("Edep");

    analysisManager->FinishNtuple(1);
}

void RunAction::EndOfRunAction(const G4Run* run){
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}

void RunAction::SetNewValue (G4UIcommand *command, G4String newValue) {
  // Filename can be set in the macro: /analysis/setFileName file_name
  // So this function may not be needed...
    if (command == m_cmdSetFileName) {
        m_fileName = newValue;
    }
    else {
        G4cerr << "Unknown command in RunAction!" << G4endl;
    }
}

void RunAction::SetNumberOfEvents(G4double numEvents){
    m_numEvents = numEvents;
}
