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
/// \file RunAction.hh
/// \brief Definition of the RunAction class

#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4Run.hh"
#include "globals.hh"
#include "PrimaryGeneratorAction.hh"

class G4Run;

/// Run action class
///

class RunAction : public G4UserRunAction, public G4UImessenger
{
  public:
    RunAction();
    virtual ~RunAction();
    void SetParticleGun(G4ParticleGun* gun);
    void SetParticleEnergy(G4double energy);
    void SetParticleID(G4int ID);
    
    //virtual void BeginOfRunAction(const G4Run*, G4double nEvt);
    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

    void SetNewValue (G4UIcommand *command, G4String newValue);
    void SetNumberOfEvents(G4double numEvents);//implement it in RunAction.cc
    
private:
    G4ParticleGun* particleGun;
    G4double m_numEvents;
    //G4String m_fileName="test_6NaI_14.5cm_10MeV";
    //G4String m_fileName="test_6NaI_15cm_10949keV";
    G4String m_fileName;
    G4UIcmdWithAString *m_cmdSetFileName = nullptr;
};


#endif

