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
/// \file exampleB4d.cc
/// \brief Main program of the B4d example
// External Libraries
#include "TFile.h"

// Geant4 Headers
#include "G4RunManagerFactory.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "DetectorConstruction.hh"
#include "B4dActionInitialization.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UItcsh.hh"
#include "G4VModularPhysicsList.hh"
#include "G4EmLivermorePhysics.hh"
#include "Randomize.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4ScoringManager.hh"


// Project-specific headers
#include "B4RunAction.hh"
#include "Levels.hh"
#include "readlevels.hh"
#include "DataBin.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " ./NaI macro levelfile num_events" << G4endl;
    //G4cerr << " exampleB4d [-m macro ] [-u UIsession] [-t nThreads]" << G4endl;
    //G4cerr << "   note: -t option is available only for multi-threaded mode."<< G4endl;
  }
}
//Modificato per la lettura dei livelli

int main( int argc, char *argv[])
{
    //Evaluate arguments

    if( argc > 4){

        PrintUsage();
        return 1;
    }

    G4double num_events; //store the number of events
    G4String macro, levelfile; //to store file name as string
    levelvec levels; //instance of the levelvec class (see Levels.hh)
    G4String session;
    //set the random seed using
    CLHEP::HepRandom::setTheSeed(std::time(0));

    macro = argv[1];
    levelfile = argv[2];
    num_events = atoi(argv[3]); //converted from a string to integer

    int nThreads = 0;
    // This part is always false since argc must be <= 4
    // if (argc == 5) {
    //   nThreads = G4UIcommand::ConvertToInt(argv[4]);
    // }

    //create a DataBin object (see DataBin.hh)

    DataBin* databin = new DataBin();

    //call the readlevels function
    if (readlevels(levelfile, &levels, databin)) return(1);


    // Detect interactive mode (if no macro provided) and define UI session

    //G4UIExecutive* ui = new G4UIExecutive(argc,argv);
    G4UIExecutive* ui = nullptr;
    if ( ! macro.size() ) {
      ui = new G4UIExecutive(argc, argv, session);
    }

    // Choose the Random engine
    //
    G4Random::setTheEngine(new CLHEP::RanecuEngine);

    // Construct the MT run manager
  auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  #ifdef G4MULTITHREADED
    //auto runManager = new G4MTRunManager;
    if ( nThreads > 0 ) {
      runManager->SetNumberOfThreads(nThreads);
    }
  //#else
  //  auto runManager = new G4RunManager;
  #endif

    //G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();

    // Set mandatory initialization classes
    auto detConstruction = new DetectorConstruction();
    runManager->SetUserInitialization(detConstruction);

    auto physicsList = new G4VModularPhysicsList();
    physicsList->RegisterPhysics(new G4EmLivermorePhysics());
    runManager->SetUserInitialization(physicsList);

    auto actionInitialization = new B4dActionInitialization(&levels, databin);
    runManager->SetUserInitialization(actionInitialization);

    //runManager->Initialize();

    // Initialize visualization
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    auto runAction = new B4RunAction();
    runAction->SetNumberOfEvents(num_events);


    if(macro == "nomac")
    {
        //runManager->Initialize();

        string word;
        G4cout << "Type vis to start visualization" << std::endl;
        cin >> word;

        if (word == "vis"){
            ui=new G4UIExecutive(argc,argv);
            // interactive mode : define UI session
            UImanager->ApplyCommand("/control/execute ../macro/init_vis.mac");
            if (ui->IsGUI()) {
              UImanager->ApplyCommand("/control/execute ../macro/gui.mac");
            }
            ui->SessionStart();
            delete ui;
        }
    }
    else
    {

        //if the macro file is provided it checks if the macro exists and the simulation runs

        std::ifstream fin;
        fin.open(macro);
        if(fin.is_open()){

            fin.close();

            runManager->Initialize();

            G4String command = "/control/execute";
            UImanager->ApplyCommand(command + macro);


            delete session;

        } else {
            G4cerr << "macro file does not exist." << G4endl;
            return(4);
        }

        ui->SessionStart();
        delete ui;

    }

    delete runAction;
    delete visManager;
    delete runManager;

    return 0;

}

