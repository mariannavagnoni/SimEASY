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
/// \file B4PrimaryGeneratorAction.cc
/// \brief Implementation of the B4PrimaryGeneratorAction class

#include "B4PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Gamma.hh"
#include "G4Event.hh"
#include "Levels.hh"
#include "readlevels.hh"
#include "CLHEP/Random/RandGeneral.h"
#include "G4SystemOfUnits.hh"
#include "DetectorConstruction.hh"
//Randomization
#include "Randomize.hh"
#include "G4RandomDirection.hh"
//PrimaryGeneratorAction
#include "G4VUserPrimaryGeneratorAction.hh"
//Defining Primary vertex
#include "G4PrimaryVertex.hh"
//Handling event user information
#include "UserEventInformation.hh"
#include <G4ParticleTable.hh>
#include <stdio.h>
#include <random>
#include <vector>
#include <string>
#include <fstream>

B4PrimaryGeneratorAction::B4PrimaryGeneratorAction(levelvec* l, DataBin* d) : G4VUserPrimaryGeneratorAction(), fParticleGun(nullptr), levels(levels), databin(databin)
{
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
  fParticleGun->SetParticleDefinition(particle);

  // Set a default value
  fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
  fParticleGun->SetParticleEnergy(4000*CLHEP::keV);
  fParticleGun->SetParticleMomentumDirection(G4RandomDirection());
}

B4PrimaryGeneratorAction::~B4PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void B4PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // This function is called at the begining of event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume
  // from G4LogicalVolumeStore

  G4double eventZ = m_length * ( G4UniformRand() - 0.5 );
  G4double eventX = 0.0;
  G4double eventY = 0.0;

  do
  {
    eventX = m_radius * (2*G4UniformRand() - 1);
    eventY = m_radius * (2*G4UniformRand() - 1);
  }
  while (eventX*eventX + eventY*eventY > m_radius*m_radius);


  G4ThreeVector particlePosition = m_center + G4ThreeVector(eventX, eventY, eventZ);
  G4cout << "eventX  " << eventX << std::endl;
  G4cout << "eventY  " << eventY << std::endl;
  G4cout << "eventZ  " << eventZ << std::endl;
  //lo stato corrispondente a i=132 è il g.s.
  // Set gun position
  fParticleGun->SetParticlePosition(particlePosition);

  fParticleGun->GeneratePrimaryVertex(anEvent);
}


