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
/// \file simEASY.cc
/// \brief Main program of the simulation for EASY

// External Libraries
#include "TFile.h"

// Geant4 Headers
#include "G4RunManagerFactory.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4UItcsh.hh"
#include "G4VModularPhysicsList.hh"
#include "G4EmLivermorePhysics.hh"
#include "Randomize.hh"
#include "G4ScoringManager.hh"

// Project-specific headers
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "Levels.hh"
#include "readlevels.hh"
#include "DataBin.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " ./simEASY macro levelfile nThreads" << G4endl;
    G4cerr << "    note: nThreads option only for multi-threaded mode." << G4endl;
  }
}
//Modificato per la lettura dei livelli

int main( int argc, char *argv[])
{
  //Evaluate arguments
  if( argc > 4 || argc < 3){
    PrintUsage();
    return 1;
  }

  G4int nThreads; // Number of threads
  G4String macro, levelfile; //to store file name as string
  levelvec levels; //instance of the levelvec class (see Levels.hh)
  G4String session;
  //set the random seed using
  CLHEP::HepRandom::setTheSeed(std::time(0));

  macro = argv[1];
  levelfile = argv[2];
  if (argc == 4) {
    nThreads = G4UIcommand::ConvertToInt(argv[3]);
  }
  else{
    G4cout << "--------------------------------------------------------------" << G4endl;
    G4cout << "---   nThreads argument not specified, using one thread.   ---" << G4endl;
    G4cout << "--------------------------------------------------------------" << G4endl;
    nThreads = 1;
  }

  //create a DataBin object (see DataBin.hh)
  DataBin* databin = new DataBin();

  //call the readlevels function
  if (readlevels(levelfile, &levels, databin)) return(1);

  // Detect interactive mode (if no macro provided) and define UI session
  G4UIExecutive* ui = nullptr;
  if ( macro == "nomac") {
    ui = new G4UIExecutive(argc, argv);
  }

  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the MT run manager
  auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  #ifdef G4MULTITHREADED
    runManager->SetNumberOfThreads(nThreads);
  #endif

  //G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();

  // Set mandatory initialization classes
  auto detConstruction = new DetectorConstruction();
  runManager->SetUserInitialization(detConstruction);

  auto physicsList = new G4VModularPhysicsList();
  physicsList->RegisterPhysics(new G4EmLivermorePhysics());
  runManager->SetUserInitialization(physicsList);

  auto actionInitialization = new ActionInitialization(&levels, databin);
  runManager->SetUserInitialization(actionInitialization);

  runManager->Initialize();

  auto runAction = new RunAction();

  if(macro == "nomac")
  {
    UImanager->ApplyCommand("/control/execute ../macro/init_vis.mac");
    if (ui->IsGUI()) {
      UImanager->ApplyCommand("/control/execute ../macro/gui.mac");
    }
    ui->SessionStart();
    delete ui;
  }
  else
  {
    //if the macro file is provided it checks if the macro exists and the simulation runs
    std::ifstream fin;
    fin.open(macro);
    if(!fin.is_open()){
      G4cerr << "macro file does not exist." << G4endl;
      return(4);
    }
    else{
      G4cout<<"<<< Macro: "<<macro<<G4endl;
      G4String command = "/control/execute ";
      UImanager->ApplyCommand(command + macro);
    } 
  }

  delete visManager;
  delete runManager;

  return 0;
}
