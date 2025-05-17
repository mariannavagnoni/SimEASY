
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
            G4cout << i << "Energy: " << levels->at(i).getEnergy() << std::endl;
        }
    }
    else{
        
       G4cout << "Error: levels vector is null!" << std::endl;
    }
}
    
PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete particleGun;
    delete[] Ed; // delete the array
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    //branching normalization
    G4double sum=0;
    //branching ratio of the gamma in the specific cascade
    G4double tot_branching=1;
    std::vector<vector<double> > cascade;
    std::vector<G4double> Egamma;
    std::vector<G4double> branch;
    //codice per la generazione del gamma primario
    //Genero casualmente un gamma nel target gassoso
    
    G4double eventZ =m_length * ( G4UniformRand() - 0.5 );
    G4double eventX=0;
    G4double eventY=0;
    
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
     //unsigned long i = 5; // Ex=11329 -> Er=832 keV in lab (unsigned long)(16*G4UniformRand()); //((levels->size()-1) * G4UniformRand());
     unsigned long i = 5; //10949.1
     G4cout << "index i: " << i << std::endl;
     
     
     G4double bin, Eg, br;
     G4double E; // energia dello stato
     G4double Ed= 0.; //livello energetico del nucleo figlio
     
     
     //G4cout << "L'ultimo livello è " << levels->at(levels->size()-1).getEnergy() << std::endl;
     int size = levels->size()-1;
    
    while( i < size )
     
    {
     
        //array branching
        G4int prova[100]={0};
        //for(G4int i=0; i<100;i++) G4cout << i << " test "<< prova[i] << std::endl;
        //getchar();
        
         //G4cout << "Bin : " << i << std::endl;
         int ssize = levels->at(i).getSize();
        
         G4int j=0;
         G4double sum=0;
         // Summation using a do-while loop
            
        G4cout <<"number of possible decay channels" << ssize << std::endl;
        
        do {
                sum += levels->at(i).getBranching()[j];
                j++;
                
        } while (j < ssize);
            
        G4cout << "Branching sum " << sum <<  std::endl;
            
        
         if(i == size)
         {
             G4cout << "Ground state reached" << std::endl;
     
             //reset
             particleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
             particleGun->SetParticleEnergy(0*CLHEP::keV);  // Set a default value
             particleGun->SetParticleMomentumDirection(G4RandomDirection());
             break;
     
         }
     
        //intbin daughter index
        G4int index=0;
        G4int intbin=0;
        int min=0;
        int max=99;
        std::vector<G4int>values;
        
        for(G4int l=0;l<ssize;l++)
       
        {
            values.push_back(int (levels->at(i).getBranching()[l]+0.5));
        }
        
        for (G4int l = 0; l < ssize ; l++) {
            
           for (G4int k = 0; k < values[l]-1; k++) {
                
                prova[index] = l;
               index++;
            }
        }
    
        //fill the prova array
        
        
        //for(G4int l=0;l<100;l++)
       
        //{
            
            //G4cout << prova[l] << std::endl;
        //}
        
         std::random_device rd;
         std::mt19937 rand(rd());
         std::uniform_int_distribution<int> unif(min,max);
         G4int seed= unif(rand);
        
         intbin=prova[seed];
         
         G4cout << "ssize" << " " << ssize << std::endl;
         G4cout << "intbin" << " " << intbin << std::endl;
         
         br=levels->at(i).getBranching()[intbin];
         Ed = levels->at(i).getDaughter()[intbin];
         
         E = levels->at(i).getEnergy();
         Eg = (E - Ed);
         
         G4cout << "The energy of the state is " << E << " keV" << std::endl;
         G4cout << "The energy of the daughter is " << Ed << " keV" << std::endl;
         G4cout << "The branching is " << br << " %" << std::endl;
         G4cout << "The gamma energy is " << Eg << " keV" << std::endl;
         
         //G4cout << "Energia dell'ultimo livello" << levels->at(levels->size()-1).getEnergy() << std::endl;
         
         
        
        
        
        
        tot_branching *= br/100;
   
        Egamma.push_back(Eg);
        branch.push_back(tot_branching*100);
        
        cascade.push_back(Egamma);
        cascade.push_back(branch);
       
        
         
        G4int primaryID = GenerateUniqueID();
        
    
        GeneratePrimaryGamma( primaryID, particlePosition, Eg,  anEvent);
        
         //condizione per la generazione del gamma secondario
         while(i <= (levels->size()-1) && Ed != levels->at(i).getEnergy() )
         {
             i++;
         
         
             if (i >= levels->size()) {
         
                 G4cerr << "Error: Index out of range!" << std::endl;
                 exit(99);
             }
         }
         
         if(Ed < 1808.74){
             G4cerr << "Groundstate reached" << std::endl;
             break;
            }
         
        
    
         }
    /*

    for(int i=0;i<cascade.size();i++){
            for(int j=0;j<cascade[i].size();j++)
                cout << cascade[i][j]<<" ";
            cout<< endl;
    }
    
    */

         
         G4cout << "After the while loop i : " << i << std::endl;
         
             
         
    
}
  

void PrimaryGeneratorAction::GeneratePrimaryGamma(G4int ID,  G4ThreeVector position, double Energy, G4Event* Event)
{
 


    G4PrimaryVertex* primaryVertex = nullptr;

    //Set the vertex properties
    particleGun->SetParticlePosition(position);
    particleGun->SetParticleEnergy(Energy*CLHEP::keV);
    particleGun->SetParticleMomentumDirection(G4RandomDirection());

    //Generate Primary Vertex
    particleGun->GeneratePrimaryVertex(Event);


    //Print additional particle properties
    const auto particleDefinition = particleGun->GetParticleDefinition();
    //G4cout << "Particle Definition: " << particleDefinition->GetParticleName() << std::endl;



    primaryVertex = Event->GetPrimaryVertex();


     const G4int nPrimaries = primaryVertex->GetNumberOfParticle();

    G4cout << "number of particles " << nPrimaries << std::endl;

    //if (nPrimaries == 1)
   // {
        const auto primaryParticle = primaryVertex->GetPrimary();
    
    
    Event->SetUserInformation(new UserEventInformation(primaryParticle->GetKineticEnergy(),
                                                           primaryVertex->GetPosition(),
                                                        primaryParticle->GetMomentum()));


    
}

G4int PrimaryGeneratorAction::GenerateUniqueID()
{
    static G4int id = 0;
    return id++;
}



