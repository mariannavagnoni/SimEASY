/*
readlevels.hh
*/

#ifndef readlevels_h
#define readlevels_h 1

#include <fstream>
#include <iostream>
#include <vector>
#include "globals.hh"
#include "Levels.hh" 
#include "DataBin.hh"
#include "G4SystemOfUnits.hh"
#include "CLHEP/Random/RandGeneral.h"
using namespace std;
using CLHEP::keV;
int readlevels(G4String, levelvec*, DataBin*);

#endif
