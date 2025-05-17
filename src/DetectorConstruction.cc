#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "G4VSensitiveDetector.hh"
#include "G4Material.hh"
#include "G4SubtractionSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4PSEnergyDeposit.hh"
#include <G4VTouchable.hh>
#include <G4PVReplica.hh>
#include "CADMesh.hh"

DetectorConstruction::DetectorConstruction(): G4VUserDetectorConstruction()
{}

DetectorConstruction::~DetectorConstruction()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4String STLpath = "";
    G4String versionSuffix = "";
    // = v1 - 3 NaI crystals =
    if (m_geometry == gVersionI)
    {
        STLpath = "../gastargetdrawing/drawings_v1/";
        versionSuffix = "";
    }
    else if (m_geometry == gVersionII)
    {
        // = v2 - 6 shorter NaI crystals, radial arrangement =
        STLpath = "../gastargetdrawing/drawings_v2/";
        versionSuffix = "_II";
    }
    else if (m_geometry == gVersionIII)
    {
        // = v3 - 6 shorter NaI crystals, orthogonal arrangement =
        STLpath = "../gastargetdrawing/drawings_v3/";
        versionSuffix = "_III";
    }
    else if (m_geometry == gVersionIV)
    {
        // = v4 - (???) =
        STLpath = "../gastargetdrawing/drawings_v4/";
        versionSuffix = "_IV";
    }
    else if (m_geometry == gVersionV)
    {
        // = v5 - (???) =
        STLpath = "../gastargetdrawing/drawings_v5/";
        versionSuffix = "_V";
    }
    else if (m_geometry == gCylinder)
    {
        // Get gas and chamber from v1
        STLpath = "../gastargetdrawing/drawings_v1/";
        versionSuffix = "";
    }
    else
    {
        G4cerr << "Unknown geometry, or geometry not set." << G4endl;
        exit(99);
    }

    // Get nist material manager
    auto nist = G4NistManager::Instance();

    G4bool checkOverlaps = true;
    G4RotationMatrix *noRotation = nullptr;

    /*World volume*/

    // a box of air
    G4VPhysicalVolume* worldPhys  = nullptr;
    G4LogicalVolume*  worldLogic = nullptr;

    const G4String worldName = "world";
    auto worldMat = nist->FindOrBuildMaterial("G4_AIR");
    const G4double worldSize = 4.0*m;

    G4VSolid* worldSolid = new G4Box(worldName, 0.5*worldSize, 0.5*worldSize, 0.5*worldSize);

    worldLogic = new G4LogicalVolume(worldSolid, worldMat, worldName);
    G4VisAttributes* invis = new G4VisAttributes();
    invis->SetVisibility(false);
    worldLogic->SetVisAttributes(invis);

    worldPhys = new G4PVPlacement(noRotation, G4ThreeVector(), worldLogic, worldName, 0, false, 0, checkOverlaps);

    /*Gas volume*/

    const G4String gasName = "gas";

    // Define gas
    const auto Ne22_atomicMass = 21.991385110*g/mole;
    auto elNe22 = new G4Element("Neon_22","Ne22", 10, Ne22_atomicMass);

    const auto R = 8.205736608e+1*cm3*atmosphere/(kelvin*mole);
    const auto Ne22gas_pressure = 3e-3*atmosphere;
    const auto Ne22gas_temperature = 293*kelvin;
    const auto Ne22gas_density = Ne22_atomicMass*Ne22gas_pressure/(R*Ne22gas_temperature);

    auto gasMat = new G4Material("Ne22_gas", Ne22gas_density, 1, kStateGas, Ne22gas_temperature, Ne22gas_pressure);
    gasMat->AddElement(elNe22, 100.*perCent);

    auto gasSTL   = CADMesh::TessellatedMesh::FromSTL(STLpath + "G4_22Ne_gas.stl");
    auto gasLogic = new G4LogicalVolume(gasSTL->GetSolid(), gasMat, gasName);

    gasLogic->SetVisAttributes(G4VisAttributes(G4Colour::Cyan()));

    new G4PVPlacement(noRotation, G4ThreeVector(), gasLogic, gasName, worldLogic, false, 0, checkOverlaps);

    /*Target chamber*/

    G4Colour myColour(113./255., 121./255., 126./255.);
    //Ash grey
    //G4Colour myColour( 178./255., 190./255., 181./255.);

    const G4String chamberName = "chamber";
    auto chamberMat = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");

    auto chamberSTL   = CADMesh::TessellatedMesh::FromSTL(STLpath + "G4_reaction_chamber.stl");
    auto chamberLogic = new G4LogicalVolume(chamberSTL->GetSolid(), chamberMat, chamberName);

    chamberLogic->SetVisAttributes(G4VisAttributes(myColour));

    new G4PVPlacement(noRotation, G4ThreeVector(), chamberLogic, chamberName, worldLogic, false, 0, checkOverlaps);

    /* Scintillator Enclosure */

    G4Colour myGrey1(192./255., 192./255., 192./255., 0.5);
    G4Colour myGrey2(221./255., 221./255., 221./255., 0.5);
    // Create a new color called myazure that is a light blue
    G4Colour myAzure(135./255., 206./255., 235./255., 0.3);
    // Create a new color called myblack that is a grey different from the gas target one
    G4Colour myGrey3(80./255., 80./255., 80./255., 0.7);

    //G4 box
    double wallThickness = 1.*CLHEP::mm;
    double px = (109./2.)*CLHEP::mm;
    double py = (109./2.)*CLHEP::mm;
    double pz = (254./2.)*CLHEP::mm;

    //Aluminum Box
    auto outerBox = new G4Box("Outer", px,py,pz);
    //Create a logical volume

    auto coverMat = nist->FindOrBuildMaterial("G4_Al");

    G4LogicalVolume *enclosure= new G4LogicalVolume(outerBox, coverMat, "Outer Box Logic",0,0,0);
    //G4VisAttributes *enclosureVis = new G4VisAttributes(myGrey2);
    G4VisAttributes *enclosureVisAtt = new G4VisAttributes(myGrey3); // change the color and transparancy of the aluminum box
	enclosureVisAtt->SetForceSolid(true);
	enclosure->SetVisAttributes(enclosureVisAtt);
    //enclosure ->SetVisAttributes(enclosureVis);

    //Air Vacuum box
    auto innerBox = new G4Box("Inner", px-wallThickness,py-wallThickness,pz-wallThickness);
    //insert vacuum between enclosure and scintillator crystal
    auto const vacuum_Z= 1.;
    auto const vacuum_A = 1.01*g/mole;
    auto const vacuum_density = 1.e-25*g/cm3; // universe_mean_density
    auto vacuumMat = new G4Material("Galactic", vacuum_Z, vacuum_A, vacuum_density, kStateGas, 273*kelvin, 3.e-18*pascal);

    G4LogicalVolume *innerBoxlogic = new G4LogicalVolume (innerBox, vacuumMat, "Inner Box Logic",0,0,0);
    //innerBoxlogic->SetVisAttributes(G4VisAttributes (myGrey1));
    innerBoxlogic->SetVisAttributes(invis);

    /* Scintillator */
    const G4String scintName = "scint";
    auto scintMat = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");
    //auto scintMat = new G4Material("NaI(Tl)", 3.667*g/cm3, 2);
    //auto activator = nist->FindOrBuildElement("G4_Tl");
    //auto SodiumIodide = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");

    //G4double activatorFraction = 0.000001;
    //G4double SodiumIodideFraction = 1.0 - activatorFraction;
    //scintMat->AddElement(activator,activatorFraction);
    //scintMat->AddMaterial(SodiumIodide,SodiumIodideFraction);

    //G4 box
    double x = (102./2.)*CLHEP::mm;
    double y = (102./2.)*CLHEP::mm;
    double z = (203./2.)*CLHEP::mm;
    auto scintSolid= new G4Box("Scint_solid", x,y,z); // its size

    auto scintLogic = new G4LogicalVolume(scintSolid, scintMat, scintName);
    //scintLogic->SetVisAttributes(G4VisAttributes(G4Colour::Cyan()));
    G4VisAttributes *scintVisAtt = new G4VisAttributes(myAzure); // change the color and transparancy of the NaI
	scintVisAtt->SetForceSolid(true);
	scintLogic->SetVisAttributes(scintVisAtt);

    G4int imax=6;
    double r = 14.5*CLHEP::cm; //nel file cad aumentiamo di 1 mm

    G4ThreeVector pos_in = G4ThreeVector(0, 0, 0.*CLHEP::mm);
    G4ThreeVector pos_scint = G4ThreeVector(0, 0, 0.*CLHEP::mm - 23.8*CLHEP::mm);

    G4PVPlacement* crystal = new G4PVPlacement(0, pos_scint, scintLogic, scintName, innerBoxlogic, false, 0, checkOverlaps);

    G4PVPlacement* vacuum = new G4PVPlacement(0, pos_in, innerBoxlogic, "Inner Box Logic", enclosure, false,  0, checkOverlaps);

    double theta=1*(360./imax)*CLHEP::degree;

    G4RotationMatrix *rot=new G4RotationMatrix();
    rot->rotateZ(-1*theta);

    G4ThreeVector pos_out = G4ThreeVector(r*cos(theta), r*sin(theta), 0.*CLHEP::mm);

    G4PVPlacement* Al_enclosure = new G4PVPlacement(rot, pos_out, enclosure, "Outer Box Logic", worldLogic, false, 1, checkOverlaps);

    //for(int i=1; i<=imax; i++){
        //double theta=i*(360./imax)*CLHEP::degree;

        //G4RotationMatrix *rot=new G4RotationMatrix();
        //rot->rotateZ(-1*theta);
//
//
        //G4ThreeVector pos_out = G4ThreeVector(r*cos(theta), r*sin(theta), 0.*CLHEP::mm);
//
//
        //G4PVPlacement* Al_enclosure = new G4PVPlacement(rot, pos_out, enclosure, "Outer Box Logic", worldLogic, false, i, checkOverlaps);

       /* if((enclosure->GetNoDaughters())== 0)

        {
            G4PVPlacement* vacuum = new G4PVPlacement(0, pos_in, innerBoxlogic, "Inner Box Logic", enclosure, false,  0, checkOverlaps);

            if((innerBoxlogic->GetNoDaughters())== 0)

            {
                G4PVPlacement* crystal = new G4PVPlacement(0, pos_scint, scintLogic, scintName, innerBoxlogic, false, 0, checkOverlaps);
            }
            else {

                G4cout << "Daughter volume already exists" << std::endl;
            }

        }
        else
        {
            G4cout << "Daughter volume already exists" << std::endl;
        }


        */

    //}

    return worldPhys;
}

void DetectorConstruction::ConstructSDandField()
{
    G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

    // declare Scintillator as a MultiFunctionalDetector scorer
    //G4MultiFunctionalDetector* mfScint = new G4MultiFunctionalDetector("scintDet");

    MySensitiveDetector* sdScint = new MySensitiveDetector("scintDet", "Edep");
    G4SDManager::GetSDMpointer()->AddNewDetector(sdScint);
    G4SDManager::GetSDMpointer()->Activate("scintDet", true);

    //G4VPrimitiveScorer* scintPrimitive = new G4PSEnergyDeposit("Edep",0);
    //mfScint->RegisterPrimitive(scintPrimitive);

    SetSensitiveDetector("scint", sdScint);
    G4cout<< "Sensitive detector " << std::endl;
}




