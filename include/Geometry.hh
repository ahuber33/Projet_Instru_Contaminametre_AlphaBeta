#ifndef Geometry_h
#define Geometry_h

/**
 * @class Geometry
 * @brief Definition of the Geometry class for the Opticalsimulation project.
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 *
 * This class provides methods to construct and retrieve different detector
 * and beamline components (volumes, chambers, quadrupoles) used in the
 * Geant4 simulation of the OpticalSimulaiton.
 */

#include "G4Box.hh"
#include "G4GDMLParser.hh"
#include "G4NistManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4UnitsTable.hh"

class OpticalSimulationGeometryConstruction;

/**
 * @class Geometry
 * @brief Provides methods to create, retrieve, and manage geometry volumes.
 *
 * The Geometry class is responsible for constructing and managing Geant4
 * geometrical objects such as quadrupoles, diagnostic chambers, and
 * GDML-defined volumes. It also provides cleanup functions for memory
 * management.
 */
class Geometry {
  public:
    /**
     * @brief Constructor for Geometry class.
     *
     * Initializes internal pointers and sets up the geometry environment.
     */
    Geometry();

    /**
     * @brief Destructor for Geometry class.
     *
     * Cleans up allocated resources.
     */
    ~Geometry();

    /**
     * @brief Load and retrieve a GDML-defined volume.
     * @param fileName Path to the GDML file.
     * @param volumeName Name of the volume to retrieve.
     * @param material Material to assign to the volume.
     * @return Pointer to the created G4LogicalVolume.
     */
    G4LogicalVolume *GetGDMLVolume(const char *fileName, const char *volumeName,
                                   G4Material *material);

    /**
     * @brief Create a Box volume.
     * @param name Name of the Box volume.
     * @param material G4Material defined
     * @param dx dx length.
     * @param dy dy length.
     * @param dz dz length.
     * @return Pointer to the created G4LogicalVolume.
     */
    G4LogicalVolume *GetBoxVolume(G4String name, G4Material *material,
                                  G4double dx, G4double dy, G4double dz);

    /**
     * @brief Create a PMT 5inches SuperNEMO-like glass.
     * @param name Name of the PMT volume.
     * @param material G4Material defined
     * @return Pointer to the created G4LogicalVolume.
     */
    G4LogicalVolume *Get5InchesPMTGlass(G4String name, G4Material *material);

    /**
     * @brief Create a PMT 5inches SuperNEMO-like photocathode.
     * @param name Name of the Photocathode volume.
     * @param material G4Material defined
     * @return Pointer to the created G4LogicalVolume.
     */
    G4LogicalVolume *Get5InchesPhotocathode(G4String name,
                                            G4Material *material);

    /**
     * @brief Create a Tubs volume.
     * @param name Name of the Box volume.
     * @param material G4Material defined
     * @param rint internal radius.
     * @param rext external radius.
     * @param dz dz length.
     * @return Pointer to the created G4LogicalVolume.
     */
    G4LogicalVolume *GetTubsVolume(G4String name, G4Material *material,
                                   G4double rint, G4double rext, G4double dz);

    /**
     * @brief Cleanup allocated geometry components.
     *
     * Frees memory for logical volumes and solid shapes.
     */
    void Cleanup();

  private:
    Geometry *Geom; ///< Pointer to another Geometry instance (if needed).
    OpticalSimulationGeometryConstruction
        *SimGeometry;     ///< Pointer to main simulation geometry.
    G4Material *Material; ///< Default material used in volumes.
    G4LogicalVolume
        *LogicalVolume;     ///< Keeps track of allocated logical volume.
    G4Box *Box;             ///< Keeps track of allocated box solid.
    G4Tubs *Tubs;           ///< Keeps track of allocated tube solid.
    G4GDMLParser *parser;   ///< Parser for GDML geometry files.
    G4VisAttributes *clear; ///< Visualization attributes (transparency).
};

#endif
