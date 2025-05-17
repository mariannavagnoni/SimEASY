//Levels.hh
#ifndef Levels_h
#define Levels_h 1

#include "globals.hh"
#include "CLHEP/Random/RandGeneral.h"
#include <vector>

class Level
{
   private:
   G4double Energy, bin;
   G4int size;
   CLHEP::RandGeneral* ran;
	
   public:
   Level(G4double, double*, double*, int);
   Level(G4double);
   ~Level();
   
   double* daughters;
   double* decayprob;

    

   G4double decay();
   G4double getEnergy(){return(Energy);}
   G4int getSize(){return(size);}
   const G4double* getDaughter() const {return daughters;}
    const G4double* getBranching() const {return decayprob;}
};

typedef std::vector<Level> levelvec;

#endif
