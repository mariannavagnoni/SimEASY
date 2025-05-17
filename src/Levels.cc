//Levels.cc

#include "Levels.hh"

//the Level constructors and the decaychain function are implemented in the following code:
Level::Level(G4double E, G4double* d, G4double* p, G4int s)
{
   Energy=E;
   size = s;
   daughters = new G4double[s];
   decayprob = new G4double[s];
   
   for(G4int i=0; i<s; i++){
      daughters[i]=d[i];
      decayprob[i]=p[i];
   }
	
   ran = new CLHEP::RandGeneral(decayprob, size, 1);
}

Level::Level(G4double E)
{
   Energy=E;
}
//Destructor
Level::~Level()
{}
//Implementation of decay functions
G4double Level::decay()
{
   if(Energy==0.0 ) return(0);
//shoot() should return a bin number. Add +1 so that we
//can return 0 if we do not decay(have reached g.s.)
   return(ran->shoot() + 1);
}
