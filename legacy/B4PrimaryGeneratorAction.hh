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
/// \file B4PrimaryGeneratorAction.hh
/// \brief Definition of the B4PrimaryGeneratorAction class

#ifndef B4PrimaryGeneratorAction_h
#define B4PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

//Modificato
#include "G4ThreeVector.hh"
#include "Levels.hh"
#include "DataBin.hh"

class G4ParticleGun;
class G4Event;

/// The primary generator action class with particle gum.
///
/// It defines a single particle which hits the calorimeter
/// perpendicular to the input face. The type of the particle
/// can be changed via the G4 build-in commands of G4ParticleGun class
/// (see the macros provided with this example).

//Modificato

///*
class B4PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    B4PrimaryGeneratorAction(levelvec* l, DataBin* d);
    virtual ~B4PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);
    void decaychain(G4Event*);

  private:
    G4double randomX, randomY;
    G4ParticleGun* fParticleGun; //pointer to a G4 service class
    levelvec* levels;
    G4ThreeVector m_center = G4ThreeVector(0,0,0);
    G4double m_length = 22.; //*CLHEP::cm;
    G4double m_radius = 1.1; //*CLHEP::cm;
    G4ThreeVector p1, p2;
    DataBin* databin;
    CLHEP::RandGeneral* randBa;
};
//*/

//Originale

/*
class B4PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  B4PrimaryGeneratorAction();
  virtual ~B4PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event* event);

  // set methods
  void SetRandomFlag(G4bool value);

private:
  G4ParticleGun*  fParticleGun; // G4 particle gun (pointer to a G4 service class)
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
*/
#endif

