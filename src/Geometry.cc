/**
 * @file Geometry.cc
 * @brief Implementation of the Geometry class for the OpticalSimulation.
 *
 * This file implements the methods defined in Geometry.hh to construct and
 * retrieve Geant4 geometry volumes used in the optical simulation framework.
 * It provides functions to:
 *   - Load GDML-defined volumes and assign materials.
 *   - Create simplified box/tubs volumes.
 *   - Create specific 5" PMT from SuperNEMO
 *
 * The Geometry class acts as a utility for building and managing
 * logical volumes in Geant4 simulations, facilitating the definition
 * of both realistic and simplified detector components.
 *
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 */

#include "Geometry.hh"

// ***********************
// Constructor
// ***********************

/**
 * @brief Default constructor.
 *
 * Initializes internal pointers and sets up the geometry environment.
 */
Geometry::Geometry() {}

// ***********************
// Destructor
// ***********************

/**
 * @brief Destructor.
 *
 * Cleans up any allocated resources associated with the geometry.
 */
Geometry::~Geometry() {}

/**
 * @brief Load and retrieve a GDML-defined volume.
 *
 * This method uses a GDML parser to read a geometry description from a GDML
 * file, retrieve the specified volume, and assign it the provided material.
 *
 * @param path Path to the GDML file.
 * @param VName Name of the volume inside the GDML file.
 * @param material Material to assign to the retrieved logical volume.
 * @return Pointer to the created G4LogicalVolume.
 */
G4LogicalVolume *Geometry::GetGDMLVolume(const char *path, const char *VName,
                                         G4Material *material) {
    G4GDMLParser *parser = new G4GDMLParser();
    // Create tessellated volume of the requested component
    parser->Clear();
    parser->Read(G4String(path), false);
    LogicalVolume = parser->GetVolume(G4String(VName));
    LogicalVolume->SetMaterial(material);

    return LogicalVolume;
}

/**
 * @brief Create a Box volume.
 *
 * Constructs a simple box volume with the given dimensions and assigns it a
 * given material.
 *
 * @param name Name of the quadrupole logical volume.
 * @param material G4Material defined
 * @param dx Size of the box along the X-axis [mm].
 * @param dy Size of the box along the Y-axis [mm].
 * @param dz Size of the box along the Z-axis [mm].
 * @return Pointer to the created G4LogicalVolume.
 */
G4LogicalVolume *Geometry::GetBoxVolume(G4String name, G4Material *material,
                                        G4double dx, G4double dy, G4double dz) {
    Box = new G4Box(name, dx / 2 * CLHEP::mm, dy / 2 * CLHEP::mm,
                    dz / 2 * CLHEP::mm);

    LogicalVolume = new G4LogicalVolume(Box, material, name, 0, 0, 0);

    return LogicalVolume;
}

/**
 * @brief Create a Tubs volume.
 *
 * Constructs a simple tubs volume with the given dimensions and assigns it a
 * given material.
 * @param name Name of the Box volume.
 * @param material G4Material defined
 * @param rint internal radius.
 * @param rext external radius.
 * @param dz dz length.
 * @return Pointer to the created G4LogicalVolume.
 */
G4LogicalVolume *Geometry::GetTubsVolume(G4String name, G4Material *material,
                                         G4double rint, G4double rext,
                                         G4double dz) {
    G4Tubs *Tubs = new G4Tubs(name, rint, rext, dz, 0, 360 * deg);

    LogicalVolume = new G4LogicalVolume(Tubs, material, name, 0, 0, 0);

    return LogicalVolume;
}

// #####################################################################
// ############ 5 INCHES PMT GEOMETRY (from Falaise BiPo) ##############
// #####################################################################

G4LogicalVolume *Geometry::Get5InchesPMTGlass(G4String name,
                                              G4Material *material) {
    // ####################### PMT GLASS SOLID #######################
    G4int N_z_plane = 30;
    G4double Phi_start = 0. * deg;
    G4double Phi_end = 2 * 180 * deg;

    const G4double z_plane[] = {
        -89. * mm,     -88.75 * mm,  -87.5 * mm,   -87.51 * mm,   -86. * mm,
        -85.4 * mm,    -84.6 * mm,   -84. * mm,    -82.5187 * mm, -7.54324 * mm,
        -2.54324 * mm, 2.45676 * mm, 7.19658 * mm, 11.0775 * mm,  13.9256 * mm,
        21.252 * mm,   30.2224 * mm, 40.0277 * mm, 50.0224 * mm,  59.6324 * mm,
        67.6679 * mm,  74.4474 * mm, 77.7374 * mm, 80.3403 * mm,  82.8689 * mm,
        84.6888 * mm,  86.3574 * mm, 87.4988 * mm, 88.4534 * mm,  89. * mm};

    const G4double r_inner[] = {
        0. * mm,      0. * mm,      0. * mm,      35.5 * mm,    36. * mm,
        36.3 * mm,    36.7 * mm,    40.5 * mm,    40.7936 * mm, 40.9457 * mm,
        40.9462 * mm, 41.6396 * mm, 44.1647 * mm, 48.4733 * mm, 52.397 * mm,
        58.0089 * mm, 61.56 * mm,   62.4995 * mm, 61.9301 * mm, 58.1609 * mm,
        51.1608 * mm, 43.0239 * mm, 38.0377 * mm, 33.0694 * mm, 26.7013 * mm,
        20.3997 * mm, 10.8782 * mm, 0. * mm,      0. * mm,      0. * mm};

    const G4double r_outer[] = {
        0. * mm,      36.5 * mm,    37. * mm,     37. * mm,     37.5 * mm,
        40.8 * mm,    41.75 * mm,   42. * mm,     42.3589 * mm, 42.4425 * mm,
        42.4425 * mm, 43.1688 * mm, 46.0797 * mm, 51.0241 * mm, 54.6315 * mm,
        59.7845 * mm, 63.1125 * mm, 64. * mm,     63.473 * mm,  59.9087 * mm,
        53.4484 * mm, 45.4762 * mm, 40.948 * mm,  36.5916 * mm, 31.0122 * mm,
        26.1237 * mm, 19.9433 * mm, 13.889 * mm,  7.70708 * mm, 0. * mm};

    G4Polycone *Polycone = new G4Polycone(name, Phi_start, Phi_end, N_z_plane,
                                          z_plane, r_inner, r_outer);

    // Logical PMT glass: material borosilicate glass
    LogicalVolume = new G4LogicalVolume(Polycone, material, name, 0, 0, 0);

    return LogicalVolume;
}

G4LogicalVolume *Geometry::Get5InchesPhotocathode(G4String name,
                                                  G4Material *material) {
    // ####################### PMT GLASS TUBE SOLID #######################
    // Solid PMT photocathode sphere
    G4int N_z_plane = 13;
    G4double Phi_start = 0. * deg;
    G4double Phi_end = 2 * 180 * deg;

    const G4double z_plane[] = {
        40.0277 * mm, 50.0224 * mm, 59.6324 * mm, 67.6679 * mm, 74.4474 * mm,
        77.7374 * mm, 80.3403 * mm, 82.8689 * mm, 84.6888 * mm, 86.3574 * mm,
        87.4988 * mm, 88.4534 * mm, 89. * mm};

    const G4double r_inner[] = {
        61.4995 * mm, 60.9301 * mm, 57.1609 * mm, 50.1608 * mm, 42.0239 * mm,
        37.0377 * mm, 32.0694 * mm, 25.7013 * mm, 19.3997 * mm, 9.8782 * mm,
        0. * mm,      0. * mm,      0. * mm};

    const G4double r_outer[] = {
        62.4995 * mm, 61.9301 * mm, 58.1609 * mm, 51.1608 * mm, 43.0239 * mm,
        38.0377 * mm, 33.0694 * mm, 26.7013 * mm, 20.3997 * mm, 10.8782 * mm,
        0. * mm,      0. * mm,      0. * mm};

    G4Polycone *Polycone = new G4Polycone(name, Phi_start, Phi_end, N_z_plane,
                                          z_plane, r_inner, r_outer);

    LogicalVolume = new G4LogicalVolume(Polycone, material, name, 0, 0, 0);

    return LogicalVolume;
}
