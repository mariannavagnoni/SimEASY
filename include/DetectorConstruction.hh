#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1
#include "globals.hh"
#include "SDscintillator.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SDManager.hh"

#include <memory>

using std::shared_ptr;
using std::make_shared;

class G4VPhysicalVolume;
class G4LogicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction, public G4UImessenger
{
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();


private:
    enum {gVersionI, gVersionII, gVersionIII, gVersionIV, gVersionV, gCylinder, gUndefined} m_geometry = gVersionV;

    shared_ptr<G4UIcmdWithAString> m_cmdSetGeometry;
    shared_ptr<G4UIcmdWithADoubleAndUnit> m_cmdSetCylVolume, m_cmdSetCylRin, m_cmdSetCylRout, m_cmdSetCylLength;

    G4double m_cylinderVolume = 0;
    G4double m_cylinderRin  = 0;
    G4double m_cylinderRout = 0;
    G4double m_cylinderLength = 0;

    G4double fWorldSize;
    G4LogicalVolume* logicCrystal;
};

#endif
