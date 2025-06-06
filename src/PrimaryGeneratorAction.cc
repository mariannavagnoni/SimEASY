
#include "PrimaryGeneratorAction.hh"
#include "G4Gamma.hh"
#include "G4Event.hh"
#include "Levels.hh"
#include "readlevels.hh"
#include "CLHEP/Random/RandGeneral.h"
#include "G4SystemOfUnits.hh"

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


PrimaryGeneratorAction::PrimaryGeneratorAction(levelvec* l, DataBin* ) : G4VUserPrimaryGeneratorAction(), particleGun(nullptr), Ed(new G4double[1])
{
    levels = l;
    firstExcitedEnergy = levels->at(levels->size()-2).getEnergy();
    //G4int nofParticles = 1;
    
    //Instantiate Primary generator
    particleGun = new G4ParticleGun(/*nofParticles*/);
    
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
    particleGun->SetParticleDefinition(particle);
    
    // Set a default value
    particleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
    particleGun->SetParticleEnergy(4000*CLHEP::keV);
    particleGun->SetParticleMomentumDirection(G4RandomDirection());

    Ed[0] = 1.0 * MeV;
    
    if(levels){
        
        for (unsigned long i = 0; i < levels->size(); ++i) {
            G4cout << i << " Energy: " << levels->at(i).getEnergy() << G4endl;
        }
    }
    else{
        
       G4cout << "Error: levels vector is null!" << G4endl;
    }
}
    
PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete particleGun;
    delete[] Ed; // delete the array
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    ///////
    // In this function branching ratio refers to the decay probability as calculated in Levels.cc
    // It is not the raw branching given in the input file
    ///////

    G4bool diagnostics = false; // Enable outputs to terminal 

    //branching normalization
    G4double branchSum = 0; // branching sum
    //branching ratio of the gamma in the specific cascade
    G4double tot_branching = 1;

    // For diagnostics
    std::vector<vector<double> > cascade; 
    std::vector<G4double> Egamma;
    std::vector<G4double> branch;

    //codice per la generazione del gamma primario
    //Genero casualmente un gamma nel target gassoso
    
    // Randomise the particle position 
    // Z-position in the gas target (beam axis)
    G4double eventZ = m_length * ( G4UniformRand() - 0.5 );

    // X and Y positions in the gas target (analogous to beam spot size)
    G4double eventX = 0;
    G4double eventY = 0;
    do
    {
        eventX = m_radius * (2*G4UniformRand() - 1);
        eventY = m_radius * (2*G4UniformRand() - 1);
    }
    while (eventX*eventX + eventY*eventY > m_radius*m_radius);
    G4ThreeVector particlePosition = m_center + G4ThreeVector(eventX, eventY, eventZ);
    
    if(diagnostics){
        G4cout << "eventX  " << eventX << " cm" << G4endl;
        G4cout << "eventY  " << eventY << " cm" << G4endl;
        G4cout << "eventZ  " << eventZ << " cm" << G4endl;
    }
    
    // Set index of the state we want to start from, counting from 0
    // Could be set in the macro instead of hard coded... will take time to implement
    // lo stato corrispondente a i=124 è il g.s. -- the state corresponding to i=124 is the ground state
    ////////////////////////////////////
    //
    // Set me!
    G4int stateIndex = 5; // Ex = 11329 keV state -> Er = 830 keV in lab, 702 keV in c.m.
    //
    ////////////////////////////////////

    if(diagnostics) G4cout << "index stateIndex: " << stateIndex << G4endl;
    
    G4double gammaEnergy = 0.; 
    G4double stateEnergy = 0.; // energia dello stato
    G4double daughterEnergy= 0.; // livello energetico del nucleo figlio
    G4double br = 0.;
    
    //if(diagnostics) G4cout << "L'ultimo livello è " << levels->at(levels->size()-1).getEnergy() << G4endl;
    G4int numLevels = levels->size()-1; // number of levels
    G4int numBranches = 0; // number of branches for a given level (state)
    G4int branchIndex = 0; // 0 = no branches, 1 = first branch, 2 = second branch, etc
    
    //intbin daughter index
    G4int prova[100] = {0}; // Sampling array for selecting branches in cascade
    G4int index = 0; // used for filling the prova array
    G4int intbin = 0; // use to select the branching path as we step through the cascade

    std::vector<G4int> values; // vector of rounded-up branching ratios for this level

    // Loop through the levels for a cascade 
    while( stateIndex < numLevels )
    {
        // Reset variables 
        //array branching
        prova[100] = {0};
        branchIndex = 0;
        branchSum = 0;
        index = 0;
        intbin = 0;
        values.clear(); 
        
        numBranches = levels->at(stateIndex).getSize();

        if(diagnostics){ 
            G4cout << "stateIndex : " << stateIndex << G4endl;
            G4cout <<"number of possible decay channels " << numBranches << G4endl;
        }

        // Count the number of possible branches at a given level
        do 
        {
            branchSum += levels->at(stateIndex).getBranching()[branchIndex];
            branchIndex++;
        } 
        while (branchIndex < numBranches);

        if(diagnostics) G4cout << "Branching sum " << branchSum <<  G4endl;

        for(G4int l=0; l<numBranches; l++)
        {
            values.push_back( G4int(levels->at(stateIndex).getBranching()[l]+0.5) ); 
            // converted branching to integer (rounded up)
            if(diagnostics) G4cout << G4int(levels->at(stateIndex).getBranching()[l]+0.5) << G4endl;
        }
        if(diagnostics) G4cout << "End loop values" << G4endl;
        
        // Filling the trial/sampling "prova" array
        // This contains 100 elements
        // Each element contains a branching index (0, 1, 2... # of branchings-1 )
        // The number of each index is normalised to the branching (decay probability)
        // For example a level with two branching possibilities, (1) 36% and (2) 64%, will have a prova array of 36x 0's and 64x 1's
        for (G4int l=0; l<numBranches; l++) { 
           for (G4int k=0; k<values[l]; k++) {
                // TC: Fixed a bug here: was previously only looping up to <values[l]-1, which meant the last elements were not correctly assigned (missing a value for every extra branch)
                prova[index] = l;
                index++;
            }
        }

        if(diagnostics){
            //output contents of the prova array to terminal
            for(G4int l=0; l<100; l++)
            {
                G4cout << prova[l] << G4endl;
            }
        }   
        
        // Generate uniform random seed for sampling the prova array
        std::random_device rd;
        std::mt19937 rand(rd());
        std::uniform_int_distribution<int> unif(0, 99);
        G4int seed = unif(rand);
        
        // Select the branching we take using the seed
        // intbin is the branching id (0, 1, 2...)
        intbin = prova[seed];
         
        if(diagnostics){ 
            G4cout << "branchIndex" << " " << branchIndex << G4endl;
            G4cout << "intbin" << " " << intbin << G4endl;
        }
         
        br = levels->at(stateIndex).getBranching()[intbin];
        daughterEnergy = levels->at(stateIndex).getDaughter()[intbin];
         
        stateEnergy = levels->at(stateIndex).getEnergy();
        gammaEnergy = (stateEnergy - daughterEnergy);
        
        if(diagnostics){
            G4cout << "The energy of the state is " << stateEnergy << " keV" << G4endl;
            G4cout << "The energy of the daughter is " << daughterEnergy << " keV" << G4endl;
            G4cout << "The branching is " << br << " %" << G4endl;
            G4cout << "The gamma energy is " << gammaEnergy << " keV" << G4endl;
            //G4cout << "Energia dell'ultimo livello" << levels->at(levels->size()-1).getEnergy() << G4endl;
        }

        tot_branching *= br/100;
   
        // Store gamma-ray energy and branching to cascade[][] for diagnostics
        if(diagnostics){
            Egamma.push_back(gammaEnergy);
            branch.push_back(tot_branching*100);
            cascade.push_back(Egamma);
            cascade.push_back(branch);
        }

        G4int primaryID = GenerateUniqueID(); 
        if(diagnostics) G4cout << "primaryID = " << primaryID << G4endl;
        //GeneratePrimaryGamma(primaryID, particlePosition, gammaEnergy,  anEvent);
        GenerateGamma(primaryID, particlePosition, gammaEnergy,  anEvent);
        
        //condizione per la generazione del gamma secondario
        // Increment stateIndex until we reach the daughter state corresponding to daughterEnergy
        while(stateIndex <= numLevels-1 && daughterEnergy != levels->at(stateIndex).getEnergy() )
        {
            stateIndex++;
         
            if (stateIndex >= numLevels+1) {
                G4cerr << "Error: stateIndex out of range!" << G4endl;
                exit(99);
            }
        }
         
        //if(daughterEnergy < 1808.74){
        if(daughterEnergy < firstExcitedEnergy){
            if(diagnostics){
                G4cout << "-------------------------------------------------" << G4endl;
                G4cout << "Daughter energy: " << daughterEnergy << " keV is below the first excited energy: " << firstExcitedEnergy << " keV" << G4endl;
                G4cout << "Groundstate reached" << G4endl;
                G4cout << "-------------------------------------------------" << G4endl;
            }
            break;
        }
         
    }
    
    if(diagnostics){
        G4cout << "Output cascade information " << G4endl;
        for(G4int i=0; i<cascade.size(); i++){
            for(G4int j=0; j<cascade[i].size(); j++){
                G4cout << cascade[i][j]<<" ";
            }
            G4cout << G4endl;
        }

        G4cout << "After the while loop stateIndex : " << stateIndex << G4endl;
    }
}

void PrimaryGeneratorAction::GeneratePrimaryGamma(G4int ID,  G4ThreeVector position, double Energy, G4Event* Event)
{
    G4PrimaryVertex* primaryVertex = nullptr;

    //Set the vertex properties
    particleGun->SetParticlePosition(position);
    particleGun->SetParticleEnergy(Energy*CLHEP::keV);
    particleGun->SetParticleMomentumDirection(G4RandomDirection());

    G4cout << "Primary gamma energy = " << Energy*CLHEP::keV << " keV" << G4endl;

    //Generate Primary Vertex
    particleGun->GeneratePrimaryVertex(Event);

    //Print additional particle properties
    //const auto particleDefinition = particleGun->GetParticleDefinition();
    //G4cout << "Particle Definition: " << particleDefinition->GetParticleName() << G4endl;

    primaryVertex = Event->GetPrimaryVertex();
    const G4int nPrimaries = primaryVertex->GetNumberOfParticle();
    // G4cout << "number of particles " << nPrimaries << G4endl;

    const auto primaryParticle = primaryVertex->GetPrimary();

    G4cout << "Primary gamma energy saved to the root file = " << primaryParticle->GetKineticEnergy() << " keV" << G4endl;
    
    Event->SetUserInformation(new UserEventInformation(primaryParticle->GetKineticEnergy(),
                                                        primaryVertex->GetPosition(),
                                                        primaryParticle->GetMomentum()
                                                    ));

}

void PrimaryGeneratorAction::GenerateGamma(G4int ID,  G4ThreeVector position, double Energy, G4Event* Event)
{
    //Set the vertex properties
    particleGun->SetParticlePosition(position);
    particleGun->SetParticleEnergy(Energy*CLHEP::keV);
    particleGun->SetParticleMomentumDirection(G4RandomDirection());

    //G4cout << "Primary gamma energy = " << Energy*CLHEP::keV << " keV" << G4endl;

    //Generate Primary Vertex
    particleGun->GeneratePrimaryVertex(Event);

    //Print additional particle properties
    //const auto particleDefinition = particleGun->GetParticleDefinition();
    //G4cout << "Particle Definition: " << particleDefinition->GetParticleName() << G4endl;

    //G4cout << "Primary gamma energy saved to the root file = " << particleGun->GetParticleEnergy() << " keV" << G4endl;
    
    Event->SetUserInformation(new UserEventInformation(particleGun->GetParticleEnergy(),
                                                        particleGun->GetParticlePosition(),
                                                        particleGun->GetParticleMomentumDirection()
                                                    ));

}

G4int PrimaryGeneratorAction::GenerateUniqueID()
{
    static G4int id = 0;
    return id++;
}
