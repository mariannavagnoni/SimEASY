#include "DetectorConstruction.hh"

#include <G4VTouchable.hh>
#include <G4PVReplica.hh>
#include "CADMesh.hh"

DetectorConstruction::DetectorConstruction(): G4VUserDetectorConstruction()
{
    fWorldSize = 3.0*m;
}

DetectorConstruction::~DetectorConstruction()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // Aluminum external box dimensions
    G4double box_length = 254*mm;
    G4double box_side = 109*mm;
    G4double box_thickness = 1*mm;

    // Aluminum internal box dimensions
    G4double box_inner_length = box_length - 2*box_thickness;
    G4double box_inner_side = box_side - 2*box_thickness;

    // NaI(Tl) crystal dimensions
    G4double crystal_side = 102*mm;
    G4double crystal_length = 203*mm;

    // Support to the base of the crystal dimensions
    G4double support_side = box_inner_side;
    G4double support_length = 3.4*mm - box_thickness;

    // Reflector external dimensions
    G4double reflector_side = box_inner_side;
    G4double reflector_length = crystal_length;

    // Reflector internal dimensions
    G4double reflector_inner_side = reflector_side - 2*(3.4*mm - box_thickness);
    G4double reflector_inner_length = reflector_length;

    // SiPM dimensions
    G4double SiPM_side = 25*mm;
    G4double SiPM_length = 1.35*mm;

    // Quartz window dimensions
    G4double quartz_side = crystal_side;
    G4double quartz_length = 12.5*mm;

    // Lead brick support dimensions
    G4double xLead = 19.8*cm;
	G4double yLead = 8.8*cm;
	G4double zLead = 5.0*cm;

    // Hole in the lead brick
    G4double xHole = 8.0 * cm;
	G4double yHole = 0.1 * cm;
	G4double zHole = 5.0 * cm;

    // Cylindrical collimator in the lead brick
    G4double rCylinder = 4.5 * mm;
	G4double zCylinder = 5.0 * cm;

    // Check geometry overlaps
	G4bool checkOverlaps = true;

    /*Variable to decide the experimental setup you want*/
    // 0 - single NaI(Tl) setup with Pb brick as source collimator
    // 1 - gas target in the center and different configurations of EASY array around it
    G4int setup = 1;

    /*Variable to decide the detector construction you want when you have seup = 1*/
    // 0 - radial configuration
    // 1 - rectangular configuration
    G4int configuration = 1;

    // For visualization
    G4VisAttributes* invis = new G4VisAttributes();
    invis->SetVisibility(false);

    // Define materials
    G4NistManager* nist = G4NistManager::Instance();

    G4Material* matPb = nist->FindOrBuildMaterial("G4_Pb");
    G4Material* matAl = nist->FindOrBuildMaterial("G4_Al");
    G4Material* matNaI = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");
    G4Material* matQuartz = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    G4Material* matMylar = nist->FindOrBuildMaterial("G4_MYLAR"); // Material of the support to the crystal base (?)
    G4Material* matAir = nist->FindOrBuildMaterial("G4_AIR");

    // Color creation for visualization
    G4Colour myGrey(192./255., 192./255., 192./255., 0.3);
    G4Colour myYellow(1.0, 1.0, 0.0, 0.3);
    G4Colour myCyan(0.5, 0.8, 0.9, 0.3);
    G4Colour myViolet(0.294, 0.0, 0.510, 0.3);
    G4Colour myBlack(0.0, 0.0, 0.0, 0.3);
    G4Colour myBrown(165.0/255.0, 42.0/255.0, 42.0/255.0, 0.3);
    G4Colour myLavander(0.902, 0.902, 0.980, 0.3);
    G4Colour myGreen(0.0, 1.0, 0.0, 0.3);

    /*World definition*/
	G4Box *solidWorld = new G4Box("solidWorld", fWorldSize/2, fWorldSize/2, fWorldSize/2);
	G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, matAir, "logicWorld");
	logicWorld->SetVisAttributes(invis);
	G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, checkOverlaps);

    if(setup == 0){

        // LEAD BRICK CONSTRUCTION
        // Pb brick with a hole and a cylindrical collimator
	    G4Box *solidLead = new G4Box("solidLead", xLead/2, yLead/2, zLead/2);
        G4Box *solidHole = new G4Box("solidHole", xHole/2, yHole/2, zHole/2);
        G4SubtractionSolid* subtraction1 = new G4SubtractionSolid("Subtraction1", solidLead, solidHole); // subraction of the hole from the lead brick
        G4Tubs *solidCylinder = new G4Tubs("solidCylinder", 0., 0.5*rCylinder, 0.5*zCylinder, 0., 360. * deg);
        G4SubtractionSolid* subtraction2 = new G4SubtractionSolid("Subtraction2", subtraction1, solidCylinder); // subraction of the cylindrical collimator from the lead brick

        // Create logical volume and physical volume using the subtracted solid volume
        G4LogicalVolume *logicLead = new G4LogicalVolume(subtraction2, matPb, "logicLead");

        // Define the Pb brick position respet to the detector one
        G4double brick_y = (box_side/2) + (zLead/2);
        G4double brick_position = -22.1*mm; // The center of the Pb brick is at the NaI(Tl) crystal

        // Angle definition to rotate the crystal
        G4RotationMatrix* rot = new G4RotationMatrix();
        rot->rotateX(90.*deg);
        rot->rotateZ(180.*deg);

        G4VPhysicalVolume *physLead = new G4PVPlacement(rot, G4ThreeVector(0., -brick_y, brick_position), logicLead, "physLead", logicWorld, false, 0, checkOverlaps);

        G4VisAttributes *leadVisAtt = new G4VisAttributes(myGrey);
        leadVisAtt->SetForceSolid(true);
        logicLead->SetVisAttributes(leadVisAtt);

        // NaI(Tl) CRYSTAL CONSTRUCTION
        // Create detector components
        G4Box* solidExternalAlBox = new G4Box("ExternalAlBox", box_side/2, box_side/2, box_length/2);
        G4Box* solidInnerAlBox = new G4Box("InnerAlBox", box_inner_side/2, box_inner_side/2, box_inner_length/2);
        G4SubtractionSolid* solidAlBox = new G4SubtractionSolid("AlBox", solidExternalAlBox, solidInnerAlBox);
        G4Box* solidExternalReflector = new G4Box("ExternalReflector", reflector_side/2, reflector_side/2, reflector_length/2);
        G4Box* solidInnerReflector = new G4Box("InnerReflector", reflector_inner_side/2, reflector_inner_side/2, reflector_inner_length/2);
        G4SubtractionSolid* solidReflector = new G4SubtractionSolid("Reflector", solidExternalReflector, solidInnerReflector);
        G4Box* solidCrystal = new G4Box("Crystal", crystal_side/2, crystal_side/2, crystal_length/2);
        G4Box* solidQuartz = new G4Box("Quartz", quartz_side/2, quartz_side/2, quartz_length/2);
        G4Box* solidSiPM = new G4Box("SiPM", SiPM_side/2, SiPM_side/2, SiPM_length/2);
        G4Box* solidSupport = new G4Box("Support", support_side/2, support_side/2, support_length/2);

        // Create logical volumes
        G4LogicalVolume* logicAlBox = new G4LogicalVolume(solidAlBox, matPb, "AlBox");
        G4LogicalVolume* logicReflector = new G4LogicalVolume(solidReflector, matMylar, "Reflector");
        logicCrystal = new G4LogicalVolume(solidCrystal, matNaI, "Crystal");
        G4LogicalVolume* logicQuartz = new G4LogicalVolume(solidQuartz, matQuartz, "Quartz");
        G4LogicalVolume* logicSiPM = new G4LogicalVolume(solidSiPM, matAl, "SiPM");
        G4LogicalVolume* logicSupport = new G4LogicalVolume(solidSupport, matAl, "Support");

        // Set visualization attributes
        G4VisAttributes *AlBoxVisAtt = new G4VisAttributes(myGrey);
        AlBoxVisAtt->SetForceSolid(true);
        logicAlBox->SetVisAttributes(AlBoxVisAtt);
        //G4VisAttributes *ReflectorVisAtt = new G4VisAttributes(myYellow);
        //ReflectorVisAtt->SetForceSolid(true);
        //logicReflector->SetVisAttributes(ReflectorVisAtt);
        logicReflector->SetVisAttributes(invis); // invisible reflector
        G4VisAttributes *CrystalVisAtt = new G4VisAttributes(myCyan);
        CrystalVisAtt->SetForceSolid(true);
        logicCrystal->SetVisAttributes(CrystalVisAtt);
        G4VisAttributes *QuartzVisAtt = new G4VisAttributes(myLavander);
        QuartzVisAtt->SetForceSolid(true);
        logicQuartz->SetVisAttributes(QuartzVisAtt);
        G4VisAttributes *SiPMVisAtt = new G4VisAttributes(myBlack);
        SiPMVisAtt->SetForceSolid(true);
        logicSiPM->SetVisAttributes(SiPMVisAtt);
        G4VisAttributes *SupportVisAtt = new G4VisAttributes(myViolet);
        SupportVisAtt->SetForceSolid(true);
        logicSupport->SetVisAttributes(SupportVisAtt);

        // Define detector component positions relative to Al box that is in the center of the world volume
        G4double albox_z = 0.0*mm;
        G4double crystal_z = -22.1*mm;
        G4double reflector_z = crystal_z;
        G4double quartz_z = 79.4*mm + (quartz_length/2);
        G4double SiPM_z = quartz_z + (quartz_length/2) + (SiPM_length/2);
        G4double support_z = 22.1*mm + (crystal_length/2) + (support_length/2);

        // Angle definition to rotate the crystal
        G4RotationMatrix* rotZ = new G4RotationMatrix();
        rotZ->rotateZ(90.*deg);

        // Place detector components
        new G4PVPlacement(rotZ, G4ThreeVector(0, 0, albox_z), logicAlBox, "AlBox", logicWorld, false, 0, true);
        new G4PVPlacement(0, G4ThreeVector(0, 0, reflector_z), logicReflector, "Reflector", logicWorld, false, 0, true);
        new G4PVPlacement(0, G4ThreeVector(0, 0, crystal_z), logicCrystal, "Crystal", logicWorld, false, 0, true);
        new G4PVPlacement(0, G4ThreeVector(0, 0, quartz_z), logicQuartz, "Quartz", logicWorld, false, 0, true);
        new G4PVPlacement(0, G4ThreeVector(0, 0, SiPM_z), logicSiPM, "SiPM", logicWorld, false, 0, true);
        new G4PVPlacement(0, G4ThreeVector(0, 0, -support_z), logicSupport, "Support", logicWorld, false, 0, true);

    }
    else{

        // CAD GAS TARGET PROJECT
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
        // CAD GAS TARGET PROJECT

        // GAS VOLUME
        const G4String gasName = "gas";

        // Define gas
        const auto Ne22_atomicMass = 21.991385110*g/mole;
        auto elNe22 = new G4Element("Neon_22","Ne22", 10, Ne22_atomicMass);

        const auto R = 8.205736608e+1*cm3*atmosphere/(kelvin*mole);
        const auto Ne22gas_pressure = 3e-3*atmosphere; // 1 atm = 1.01325 bar
        const auto Ne22gas_temperature = 293*kelvin;
        const auto Ne22gas_density = Ne22_atomicMass*Ne22gas_pressure/(R*Ne22gas_temperature);

        auto gasMat = new G4Material("Ne22_gas", Ne22gas_density, 1, kStateGas, Ne22gas_temperature, Ne22gas_pressure);
        gasMat->AddElement(elNe22, 100.*perCent);

        //auto gasSTL   = CADMesh::TessellatedMesh::FromSTL(STLpath + "G4_22Ne_gas.stl"); // This looks to be a hollow cylinder! Recommend a simple G4Tubs
        //auto gasLogic = new G4LogicalVolume(gasSTL->GetSolid(), gasMat, gasName);
        auto gasSolid = new G4Tubs("gasSolid",0.0*mm, (25.2/2.)*mm, (220.0/2.)*mm, 0.0*deg, 360.0*deg); // inner diameter of target chamber = 25.4mm
        auto gasLogic = new G4LogicalVolume(gasSolid, gasMat, gasName);

        gasLogic->SetVisAttributes(G4VisAttributes(G4Colour::Cyan()));

        new G4PVPlacement(0, G4ThreeVector(), gasLogic, gasName, logicWorld, false, 0, checkOverlaps);

        // TARGET CHAMBER

        G4Colour myColour(113./255., 121./255., 126./255.);
        //Ash grey
        //G4Colour myColour( 178./255., 190./255., 181./255.);

        const G4String chamberName = "chamber";
        auto chamberMat = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");

        bool chamberDetailed = true; // set true to include more details on the target chamber
        auto chamberSTL   = CADMesh::TessellatedMesh::FromSTL(STLpath + "G4_reaction_chamber.stl");
        if(chamberDetailed) chamberSTL = CADMesh::TessellatedMesh::FromSTL(STLpath + "G4_gas_target_Detail_Low_NoInterference_Rotated_Repaired.stl");
        auto chamberLogic = new G4LogicalVolume(chamberSTL->GetSolid(), chamberMat, chamberName);

        chamberLogic->SetVisAttributes(G4VisAttributes(myColour));

        new G4PVPlacement(0, G4ThreeVector(), chamberLogic, chamberName, logicWorld, false, 0, checkOverlaps);

        // NaI(Tl) CRYSTAL CONSTRUCTION
        // Create detector components
        G4Box* solidExternalAlBox = new G4Box("ExternalAlBox", box_side/2, box_side/2, box_length/2);
        G4Box* solidInnerAlBox = new G4Box("InnerAlBox", box_inner_side/2, box_inner_side/2, box_inner_length/2);
        G4SubtractionSolid* solidAlBox = new G4SubtractionSolid("AlBox", solidExternalAlBox, solidInnerAlBox);
        G4Box* solidExternalReflector = new G4Box("ExternalReflector", reflector_side/2, reflector_side/2, reflector_length/2);
        G4Box* solidInnerReflector = new G4Box("InnerReflector", reflector_inner_side/2, reflector_inner_side/2, reflector_inner_length/2);
        G4SubtractionSolid* solidReflector = new G4SubtractionSolid("Reflector", solidExternalReflector, solidInnerReflector);
        G4Box* solidCrystal = new G4Box("Crystal", crystal_side/2, crystal_side/2, crystal_length/2);
        G4Box* solidQuartz = new G4Box("Quartz", quartz_side/2, quartz_side/2, quartz_length/2);
        G4Box* solidSiPM = new G4Box("SiPM", SiPM_side/2, SiPM_side/2, SiPM_length/2);
        G4Box* solidSupport = new G4Box("Support", support_side/2, support_side/2, support_length/2);

        // Create logical volumes
        G4LogicalVolume* logicAlBox = new G4LogicalVolume(solidAlBox, matPb, "AlBox");
        G4LogicalVolume* logicReflector = new G4LogicalVolume(solidReflector, matMylar, "Reflector");
        logicCrystal = new G4LogicalVolume(solidCrystal, matNaI, "Crystal");
        G4LogicalVolume* logicQuartz = new G4LogicalVolume(solidQuartz, matQuartz, "Quartz");
        G4LogicalVolume* logicSiPM = new G4LogicalVolume(solidSiPM, matAl, "SiPM");
        G4LogicalVolume* logicSupport = new G4LogicalVolume(solidSupport, matAl, "Support");

        // Set visualization attributes
        G4VisAttributes *AlBoxVisAtt = new G4VisAttributes(myGrey);
        AlBoxVisAtt->SetForceSolid(true);
        logicAlBox->SetVisAttributes(AlBoxVisAtt);
        //G4VisAttributes *ReflectorVisAtt = new G4VisAttributes(myYellow);
        //ReflectorVisAtt->SetForceSolid(true);
        //logicReflector->SetVisAttributes(ReflectorVisAtt);
        logicReflector->SetVisAttributes(invis); // invisible reflector
        G4VisAttributes *CrystalVisAtt = new G4VisAttributes(myCyan);
        CrystalVisAtt->SetForceSolid(true);
        logicCrystal->SetVisAttributes(CrystalVisAtt);
        G4VisAttributes *QuartzVisAtt = new G4VisAttributes(myLavander);
        QuartzVisAtt->SetForceSolid(true);
        logicQuartz->SetVisAttributes(QuartzVisAtt);
        G4VisAttributes *SiPMVisAtt = new G4VisAttributes(myBlack);
        SiPMVisAtt->SetForceSolid(true);
        logicSiPM->SetVisAttributes(SiPMVisAtt);
        G4VisAttributes *SupportVisAtt = new G4VisAttributes(myViolet);
        SupportVisAtt->SetForceSolid(true);
        logicSupport->SetVisAttributes(SupportVisAtt);

        // Define detector component positions relative to Al box that is in the center of the world volume
        G4double albox_z = 0.0*mm;
        G4double crystal_z = -22.1*mm;
        G4double reflector_z = crystal_z;
        G4double quartz_z = 79.4*mm + (quartz_length/2);
        G4double SiPM_z = quartz_z + (quartz_length/2) + (SiPM_length/2);
        G4double support_z = 22.1*mm + (crystal_length/2) + (support_length/2);

        // RADIAL CONFIGURATION
        if(configuration == 0){

            G4double radius = 150.0 * mm;
            G4int imax = 6;

            for (int i = 0; i < imax; ++i) {

                G4double angle = i * (360.0/imax) * deg;

                G4double x = radius * std::cos(angle);
                G4double y = radius * std::sin(angle);
                G4double z = 0.0 * mm;

                G4RotationMatrix* rot = new G4RotationMatrix();
                rot->rotateZ(-1*angle);

                G4ThreeVector posAlBox(x, y, z + 0.0*mm);
                G4ThreeVector posReflector(x, y, z + (-22.1*mm));
                G4ThreeVector posCrystal(x, y, z + (-22.1*mm));
                G4ThreeVector posQuartz(x, y, z + 79.4*mm + (quartz_length/2));
                G4ThreeVector posSiPM(x, y, z + quartz_z + (quartz_length/2) + (SiPM_length/2));
                G4ThreeVector posSupport(x, y, z - (22.1*mm + (crystal_length/2) + (support_length/2)));

                new G4PVPlacement(rot, posAlBox, logicAlBox, "AlBox", logicWorld, false, i, true);
                new G4PVPlacement(rot, posReflector, logicReflector, "Reflector", logicWorld, false, i, true);
                new G4PVPlacement(rot, posCrystal, logicCrystal, "Crystal", logicWorld, false, i, true);
                new G4PVPlacement(rot, posQuartz, logicQuartz, "Quartz", logicWorld, false, i, true);
                new G4PVPlacement(rot, posSiPM, logicSiPM, "SiPM", logicWorld, false, i, true);
                new G4PVPlacement(rot, posSupport, logicSupport, "Support", logicWorld, false, i, true);
            }
        }

        // RECTANGULAR CONFIGURATION
        if(configuration == 1){

            G4double d = 115.0 * mm;
            G4double offset_3_4 = 0.0 * mm;

            // vector of vectors, each vector inside is the position of a single crystal in the configuration
            std::vector<G4ThreeVector> positions = {
                G4ThreeVector(-d/2,  d, 0),  // modulo 1 (alto sinistra)
                G4ThreeVector( d/2,  d, 0),  // modulo 2 (alto destra)
                G4ThreeVector(-d+offset_3_4, 0, 0),  // modulo 3 (sinistra)
                G4ThreeVector( d-offset_3_4, 0, 0),  // modulo 4 (destra)
                G4ThreeVector(-d/2, -d, 0),  // modulo 5 (basso sinistra)
                G4ThreeVector( d/2, -d, 0)   // modulo 6 (basso destra)
            };

            for (size_t i = 0; i < positions.size(); ++i) {

                G4RotationMatrix* rot = new G4RotationMatrix();
                rot->rotateZ(0); // oppure usa `0` se non vuoi rotazione

                G4ThreeVector posAlBox = positions[i] + G4ThreeVector(0, 0, albox_z);
                G4ThreeVector posReflector = positions[i] + G4ThreeVector(0, 0, reflector_z);
                G4ThreeVector posCrystal = positions[i] + G4ThreeVector(0, 0, crystal_z);
                G4ThreeVector posQuartz = positions[i] + G4ThreeVector(0, 0, quartz_z);
                G4ThreeVector posSiPM = positions[i] + G4ThreeVector(0, 0, SiPM_z);
                G4ThreeVector posSupport = positions[i] + G4ThreeVector(0, 0, -support_z);

                new G4PVPlacement(rot, posAlBox, logicAlBox, "AlBox", logicWorld, false, i, true);
                new G4PVPlacement(rot, posReflector, logicReflector, "Reflector", logicWorld, false, i, true);
                new G4PVPlacement(rot, posCrystal, logicCrystal, "Crystal", logicWorld, false, i, true);
                new G4PVPlacement(rot, posQuartz, logicQuartz, "Quartz", logicWorld, false, i, true);
                new G4PVPlacement(rot, posSiPM, logicSiPM, "SiPM", logicWorld, false, i, true);
                new G4PVPlacement(rot, posSupport, logicSupport, "Support", logicWorld, false, i, true);
            }

        }

    }

    return physWorld;
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

    logicCrystal->SetSensitiveDetector(sdScint); //"scint" is the logic volume for array configuration

}





