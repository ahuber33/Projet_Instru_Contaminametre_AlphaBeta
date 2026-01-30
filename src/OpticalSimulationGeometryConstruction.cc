/**
 * @file OpticalSimulationGeometryConstruction.cc
 * @brief Implements the detector geometry and magnetic field setup for the
 * Optical simulation.
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 *
 * This file contains the method definitions for the
 * `OpticalSimulationGeometryConstruction` class declared in
 * `OpticalSimulationGeometryConstruction.hh`. It manages:
 *  - Construction of the full and simplified detector geometry
 *  - Loading of GDML models for realistic component shapes
 *  - Definition of visualization attributes for logical volumes
 *  - Creation of world and holder volumes
 *  - Assignment of materials to all volumes
 *  - Optional multithread-safe handling of magnetic field instances
 *
 * The geometry construction workflow:
 *  - **Construct()**:
 *      - Cleans any existing geometry to prevent duplication
 *      - Defines rotation matrices for volume placement
 *      - Calls `CreateWorldAndHolder()` to create the world and main holder
 * volume
 *
 * Thread safety is ensured via:
 *  - `G4Mutex fieldManagerMutex` for synchronized access to the magnetic field
 * manager
 *  - `G4ThreadLocal` instances of `OpticalSimulationMagneticField` and
 * `G4FieldManager`
 *
 * Visualization colors for logical volumes:
 *  - "invis", "black", "white", "gray", "red", "orange", "yellow", "green",
 * "cyan", "blue", "magenta"
 *
 */

#include "OpticalSimulationGeometryConstruction.hh"

using namespace CLHEP;

//! Mutex to synchronize access to the magnetic field manager in multithreaded
//! mode
const G4String OpticalSimulationGeometryConstruction::path =
    "../simulation_input_files/";
std::mutex geometryMutex;
std::mutex fileAccessMutex;

/**
 * @brief Constructor for OpticalSimulationGeometryConstruction.
 *
 * Initializes the base geometry and attaches the geometry messenger
 * for interactive user control via macro commands.
 */
OpticalSimulationGeometryConstruction::OpticalSimulationGeometryConstruction()
    : G4VUserDetectorConstruction() {
    Geom = std::make_unique<Geometry>();
    fGeometryMessenger =
        std::make_unique<OpticalSimulationGeometryMessenger>(this);
}

/**
 * @brief Destructor for OpticalSimulationGeometryConstruction.
 */
OpticalSimulationGeometryConstruction::
    ~OpticalSimulationGeometryConstruction() = default;

/**
 * @brief Print a summary of the current geometry setup.
 */
void OpticalSimulationGeometryConstruction::Print() {
    G4cout << "\n------------------------------------------------------"
           << G4endl;
    G4cout << "-----------------------------------------------------" << G4endl;
}

/**
 * @brief Assign a visualization color to a logical volume.
 *
 * Predefined colors can be applied to logical volumes for debugging or
 * visualization.
 *
 * @param LogicalVolume Pointer to the logical volume to colorize.
 * @param Color Name of the color (e.g. "red", "green", "gray").
 */
void OpticalSimulationGeometryConstruction::SetLogicalVolumeColor(
    G4LogicalVolume *LogicalVolume, G4String Color) {
    // ***********************
    // Visualization Colors
    // ***********************
    invis = new G4VisAttributes(G4Colour(255 / 255., 255 / 255., 255 / 255.));
    invis->SetVisibility(false);

    black = new G4VisAttributes(G4Colour(0, 0, 0, 0.9));
    black->SetForceSolid(true);
    black->SetVisibility(true);

    white = new G4VisAttributes(G4Colour(1, 1, 1, 1.));
    white->SetForceSolid(true);
    white->SetVisibility(true);

    gray = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.5));
    gray->SetVisibility(true);

    red = new G4VisAttributes(G4Colour(1, 0, 0, 0.3));
    red->SetForceSolid(true);
    red->SetVisibility(true);

    orange = new G4VisAttributes(G4Colour(1, 0.5, 0, 0.3));
    orange->SetForceSolid(true);
    orange->SetVisibility(true);

    yellow = new G4VisAttributes(G4Colour(1, 1, 0, 0.5));
    yellow->SetForceSolid(true);
    yellow->SetVisibility(true);

    green = new G4VisAttributes(G4Colour(0, 1, 0, 0.3));
    green->SetForceSolid(true);
    green->SetVisibility(true);

    cyan = new G4VisAttributes(G4Colour(0, 1, 1, 0.3));
    cyan->SetForceSolid(true);
    cyan->SetVisibility(true);

    blue = new G4VisAttributes(G4Colour(0, 0, 1, 0.5));
    blue->SetForceSolid(true);
    blue->SetVisibility(true);

    magenta = new G4VisAttributes(G4Colour(1, 0, 1, 0.3));
    magenta->SetVisibility(true);

    // Apply selected color
    if (Color == "invis") {
        LogicalVolume->SetVisAttributes(invis);
    } else if (Color == "black") {
        LogicalVolume->SetVisAttributes(black);
    } else if (Color == "white") {
        LogicalVolume->SetVisAttributes(white);
    } else if (Color == "gray") {
        LogicalVolume->SetVisAttributes(gray);
    } else if (Color == "red") {
        LogicalVolume->SetVisAttributes(red);
    } else if (Color == "orange") {
        LogicalVolume->SetVisAttributes(orange);
    } else if (Color == "yellow") {
        LogicalVolume->SetVisAttributes(yellow);
    } else if (Color == "green") {
        LogicalVolume->SetVisAttributes(green);
    } else if (Color == "cyan") {
        LogicalVolume->SetVisAttributes(cyan);
    } else if (Color == "blue") {
        LogicalVolume->SetVisAttributes(blue);
    } else if (Color == "magenta") {
        LogicalVolume->SetVisAttributes(magenta);
    }
}

/**
 * @brief Create the world and holder volumes.
 *
 * Defines the simulation world as a large vacuum box and
 * places a holder volume inside it for containing components.
 */
void OpticalSimulationGeometryConstruction::CreateWorldAndHolder() {
    auto VacuumWorld =
        OpticalSimulationMaterials::getInstance()->getMaterial("VacuumWorld");
    auto Vacuum =
        OpticalSimulationMaterials::getInstance()->getMaterial("Vacuum");

    G4Box *SolidWorld = new G4Box("SolidWorld", 2.1 * m, 15.1 * m, 2.1 * m);
    LogicalWorld = new G4LogicalVolume(SolidWorld, VacuumWorld, "LogicalWorld");
    SetLogicalVolumeColor(LogicalWorld, "invis");

    G4Box *SolidHolder =
        new G4Box("SolidHolder", 2.05 * m, 15.05 * m, 2.05 * m);
    LogicalHolder = new G4LogicalVolume(SolidHolder, Vacuum, "LogicalHolder");
    SetLogicalVolumeColor(LogicalHolder, "invis");

    PhysicalWorld =
        new G4PVPlacement(G4Transform3D(DontRotate, G4ThreeVector(0, 0, 0)),
                          "World", LogicalWorld, NULL, false, 0);

    PhysicalHolder =
        new G4PVPlacement(G4Transform3D(DontRotate, G4ThreeVector(0, 0, 0)),
                          LogicalHolder, "Holder", LogicalWorld, false, 0);
}

/**
 * @brief Construct the ZnS:Ag part.
 */
void OpticalSimulationGeometryConstruction::ConstructZnS() {
    auto ZnS = OpticalSimulationMaterials::getInstance()->getMaterial("ZnS");

    G4MaterialPropertiesTable *mpt = ZnS->GetMaterialPropertiesTable();
    mpt->AddConstProperty("SCINTILLATIONYIELD", fZnSLY / MeV, true);

    LogicalZnS =
        Geom->GetBoxVolume("ZnS", ZnS, fZnSLength, fZnSWidth, fZnSThickness);

    // Assign colors
    SetLogicalVolumeColor(LogicalZnS, "black");

    PhysicalZnS = new G4PVPlacement(
        G4Transform3D(DontRotate, G4ThreeVector(0. * mm, 0 * mm, 0 * mm)),
        LogicalZnS, "ZnS", LogicalHolder, false, 0);
}

/**
 * @brief Construct the Scintillator part.
 */
void OpticalSimulationGeometryConstruction::ConstructScintillator() {
    auto EJ212 =
        OpticalSimulationMaterials::getInstance()->getMaterial("EJ212");

    G4MaterialPropertiesTable *mpt = EJ212->GetMaterialPropertiesTable();
    mpt->AddConstProperty("SCINTILLATIONYIELD", fScintillatorLY / MeV, true);

    LogicalScintillator =
        Geom->GetBoxVolume("Scintillator", EJ212, fScintillatorLength,
                           fScintillatorWidth, fScintillatorThickness);

    // Assign colors
    SetLogicalVolumeColor(LogicalScintillator, "cyan");

    PhysicalScintillator = new G4PVPlacement(
        G4Transform3D(DontRotate, G4ThreeVector(0. * mm, 0 * mm,
                                                fScintillatorThickness / 2 +
                                                    fZnSThickness / 2)),
        LogicalScintillator, "Scintillator", LogicalHolder, false, 0);
}

/**
 * @brief Construct the PMT Glass part.
 */
void OpticalSimulationGeometryConstruction::ConstructPMTGlass() {
    auto bs_glass =
        OpticalSimulationMaterials::getInstance()->getMaterial("bs_glass");

    LogicalPMTGlass = Geom->Get5InchesPMTGlass("PMT_Glass", bs_glass);

    // Assign colors
    SetLogicalVolumeColor(LogicalPMTGlass, "yellow");

    PhysicalPMTGlass = new G4PVPlacement(
        G4Transform3D(Flip, G4ThreeVector(0. * mm, 0 * mm,
                                          fScintillatorThickness / 2 +
                                              fZnSThickness / 2 +
                                              fDetectorDistance + 89.5)),
        LogicalPMTGlass, "PMT_Glass", LogicalHolder, false, 0);
}

/**
 * @brief Construct the TeflonOpticalProperties.
 */
void OpticalSimulationGeometryConstruction::CreateTeflonOpticalProperties() {
    auto TeflonMPT = new G4MaterialPropertiesTable();
    // Define Teflon properties
    // Properties are read in from data file
    std::ifstream Read_teflon;
    G4String teflon_file = path + "teflon.dat";
    std::vector<G4double> Teflon_Energy;
    std::vector<G4double> Teflon_Reflectivity;
    std::vector<G4double> Teflon_Zero;
    G4double wavelength;       // x values
    G4double teflon_ref_coeff; // y values
                               // Read_teflon.open(teflon_file);
    if (Read_teflon.is_open()) {
        while (!Read_teflon.eof()) {
            G4String filler; // This just skips the coma and space in data files
            Read_teflon >> wavelength >> filler >> teflon_ref_coeff;
            Teflon_Energy.push_back((1240 / wavelength) * eV);
            Teflon_Reflectivity.push_back(1. * teflon_ref_coeff);
            Teflon_Zero.push_back(1e-6);
        }
    } else
        G4cout << "Error opening file: " << teflon_file
               << G4endl; // throw an error if file is not found
    Read_teflon.close();

    // Define Teflon optical boundary surface properties
    auto OpticalTeflon = new G4OpticalSurface("OpticalTeflon");
    OpticalTeflon->SetModel(unified); // Either glisur (GEANT3 model) or unified
                                      // surface model: set references online
    OpticalTeflon->SetPolish(0);      // Set level of polish
    OpticalTeflon->SetFinish(
        groundfrontpainted); // Sets finish to one of 6 options: see references
                             // online - based off polished or grounded
    OpticalTeflon->SetType(
        dielectric_metal); // Sets boundary type: dielectric-dielectric or
                           // dielectric-metal
    // Define Teflon material properties
    TeflonMPT->AddProperty("REFLECTIVITY", Teflon_Energy, Teflon_Reflectivity);
    TeflonMPT->AddProperty("SPECULARLOBECONSTANT", Teflon_Energy, Teflon_Zero);
    TeflonMPT->AddProperty("SPECULARSPIKECONSTANT", Teflon_Energy, Teflon_Zero);
    TeflonMPT->AddProperty("BACKSCATTERCONSTANT", Teflon_Energy, Teflon_Zero);
    // Geometrical implementation of boundary surface
    OpticalTeflon->SetMaterialPropertiesTable(TeflonMPT);
    // auto SSTeflon = new
    // G4LogicalSkinSurface("b_Teflon",LogicalTeflon,OpticalTeflon); // Applies
    // optical Teflon properties to ogical Teflon surface
}

/**
 * @brief Construct the MylarOpticalProperties.
 */
void OpticalSimulationGeometryConstruction::CreateMylarOpticalProperties() {
    auto MylarMPT = new G4MaterialPropertiesTable();
    // Define Mylar properties
    std::ifstream Read_mylar;
    G4double wavelength; // x values
    G4String mylar_file = path + "mylar.dat";
    std::vector<G4double> Mylar_Energy;
    std::vector<G4double> Mylar_Reflectivity;
    std::vector<G4double> Mylar_Zero;
    std::vector<G4double> Mylar_Un;
    G4double mylar_ref_coeff;
    Read_mylar.open(mylar_file);
    if (Read_mylar.is_open()) {
        while (!Read_mylar.eof()) {
            G4String filler;
            Read_mylar >> wavelength >> filler >> mylar_ref_coeff;
            Mylar_Energy.push_back((1240 / wavelength) * eV);
            Mylar_Reflectivity.push_back(1. * mylar_ref_coeff);
            Mylar_Zero.push_back(0.0);
            Mylar_Un.push_back(1.0);
        }
    } else
        G4cout << "Error opening file: " << mylar_file << G4endl;
    Read_mylar.close();

    // Define Mylar optical boundary surface properties
    auto OpticalMylar = new G4OpticalSurface("OpticalMylar");
    OpticalMylar->SetModel(glisur);
    OpticalMylar->SetFinish(polished);
    OpticalMylar->SetType(dielectric_metal);

    // Define Mylar material properties
    MylarMPT->AddProperty("REFLECTIVITY", Mylar_Energy, Mylar_Reflectivity);
    MylarMPT->AddProperty("SPECULARLOBECONSTANT", Mylar_Energy, Mylar_Un);
    MylarMPT->AddProperty("SPECULARSPIKECONSTANT", Mylar_Energy, Mylar_Un);
    // MylarMPT->AddProperty("DIFFUSELOBECONSTANT",mylar_energy,realzero,mylar_entries);
    MylarMPT->AddProperty("BACKSCATTERCONSTANT", Mylar_Energy, Mylar_Zero);

    // Geometrical implementation of boundary surface
    OpticalMylar->SetMaterialPropertiesTable(MylarMPT);

    // SSMylar = new G4LogicalSkinSurface("SSMylar", LogicalMylar,OpticalMylar);
    // // Applies Mylar properties to logical mylar surface
    // auto SSAlu = new G4LogicalSkinSurface("SSAlu", LogicalBoitierAlu,
    // OpticalMylar); // Applies Mylar properties to logical Alu surface
}

/**
 * @brief Construct the DetectionOpticalProperties.
 */
void OpticalSimulationGeometryConstruction::CreateDetectionOpticalProperties() {
    auto Vacuum =
        OpticalSimulationMaterials::getInstance()->getMaterial("Vacuum");

    LogicalPhotocathode = Geom->Get5InchesPhotocathode("Photocathode", Vacuum);

    SetLogicalVolumeColor(LogicalPhotocathode, "orange");

    // Define PMT properties
    G4double wavelength;        // x values
    G4double indexconst = 1.49; // Index currently set to constant for PMT glass
    G4double reflectconst = 0.;
    std::ifstream ReadPMT;
    G4String PMTfile = path + "QE_ham_GA0154.txt";
    std::vector<G4double> Photocathode_Energy;
    std::vector<G4double> Photocathode_Value;
    std::vector<G4double> Photocathode_Index;
    std::vector<G4double> Photocathode_Reflectivity;
    G4double quant_eff;
    ReadPMT.open(PMTfile);
    if (ReadPMT.is_open()) {
        while (!ReadPMT.eof()) {
            G4String filler;
            ReadPMT >> wavelength >> filler >> quant_eff;
            Photocathode_Energy.push_back((1240 / wavelength) * eV);
            // Photocathode_Value.push_back(1);  //For the GAxxxx file with good
            // QE and 0.65 for collection efficiency
            Photocathode_Value.push_back(
                0.64 * quant_eff *
                0.65); // Change 1 if you know ou want to apply a collection
                       // efficiency factor !!!

            Photocathode_Index.push_back(indexconst);
            Photocathode_Reflectivity.push_back(reflectconst);
        }
    } else
        G4cout << "Error opening file: " << PMTfile << G4endl;
    ReadPMT.close();

    // Define PMT optical boundary surface properties
    auto OpticalPMT = new G4OpticalSurface("OpticalPMT");
    OpticalPMT->SetModel(glisur);
    OpticalPMT->SetFinish(polished);
    OpticalPMT->SetType(dielectric_metal);

    // Define PMT material properties
    auto PMT_MPT = new G4MaterialPropertiesTable();
    PMT_MPT->AddProperty("EFFICIENCY", Photocathode_Energy, Photocathode_Value);
    PMT_MPT->AddProperty("REFLECTIVITY", Photocathode_Energy,
                         Photocathode_Reflectivity);
    PMT_MPT->AddProperty("RINDEX", Photocathode_Energy, Photocathode_Index);

    // Geometrical implementation of boundary surface
    OpticalPMT->SetMaterialPropertiesTable(PMT_MPT);
    auto SSPhotocathode =
        new G4LogicalSkinSurface("DetectorSS", LogicalPhotocathode, OpticalPMT);

    // PMT photocathode placement
    PhysicalPhotocathode = new G4PVPlacement(
        G4Transform3D(Flip, G4ThreeVector(0. * mm, 0 * mm,
                                          fScintillatorThickness / 2 +
                                              fZnSThickness / 2 +
                                              fDetectorDistance + 89.5)),
        LogicalPhotocathode, "Photocathode", LogicalHolder, false, 0);
}

/**
 * @brief Construct the full detector geometry for the simulation.
 *
 * This method initializes and builds all the physical volumes required
 * for the simulation. It cleans up any previously defined geometry,
 * applies common rotation matrices, and constructs the detector
 * components according to display flags.
 *
 * Steps performed:
 * - Clean existing geometry and volume stores to avoid duplication.
 * - Define common rotation matrices used for detector components.
 * - Create the world volume and geometry holder.
 * - Create Zns & Scintillator part
 * - Create PMT Glass part
 * - Create Teflon, Mylar & Detection part
 * - Create Optical Surface
 * - Return the fully initialized world volume.
 *
 * @return Pointer to the top-level physical volume (`PhysicalWorld`)
 *         containing the entire detector setup.
 */
G4VPhysicalVolume *OpticalSimulationGeometryConstruction::Construct() {
    // --- Cleanup of previous geometry ----------------------------------------
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    // --- Define common rotation matrices -------------------------------------
    DontRotate.rotateX(0.0 * deg);
    Flip.rotateZ(0 * deg);
    Flip.rotateX(90 * deg);
    Flip.rotateY(0 * deg);

    // #########################################################################
    //  DEFINE GEOMETRY VOLUMES
    // #########################################################################

    /// Create the world and main holder volume
    CreateWorldAndHolder();

    ConstructScintillator();

    ConstructZnS();

    ConstructPMTGlass();

    CreateDetectionOpticalProperties();

    G4OpticalSurface *surface = new G4OpticalSurface("ScintillatorToHolder");
    surface->SetType(dielectric_dielectric);
    surface->SetFinish(polished);
    surface->SetModel(unified); // modèle plus complet
    new G4LogicalBorderSurface("SurfScintHolder", PhysicalScintillator,
                               PhysicalHolder, surface);

    G4cout << "END OF THE DETECTOR CONSTRUCTION" << G4endl;

    // --- Return the fully constructed world volume ---------------------------
    return PhysicalWorld;
}
