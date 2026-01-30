#ifndef OpticalSimulationGeometryConstruction_h
#define OpticalSimulationGeometryConstruction_h 1

/**
 * @class OpticalSimulationGeometryConstruction
 * @brief Defines the detector geometry for the OpticalSimulation
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 *
 * This class is responsible for:
 *  - Building the world and detector components.
 *  - Setting visualization attributes.
 *  - Providing user control over geometry display.
 */

#include "G4GeometryManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4SurfaceProperty.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VisAttributes.hh"
#include "Geometry.hh"
#include "OpticalSimulationGeometryMessenger.hh"
#include "OpticalSimulationMaterials.hh"

class Geometry;
class G4FieldManager;
class OpticalSimulationGeometryMessenger;
class OpticalSimulationMagneticField;

class OpticalSimulationGeometryConstruction final
    : public G4VUserDetectorConstruction {
  public:
    /** @brief Default constructor. */
    OpticalSimulationGeometryConstruction();

    /** @brief Destructor. */
    ~OpticalSimulationGeometryConstruction() override;

    /** @name Geometry Construction */
    ///@{
    /** @brief Set color for a given logical volume. */
    void SetLogicalVolumeColor(G4LogicalVolume *LogicalVolume, G4String color);

    /** @brief Create world and base holder. */
    void CreateWorldAndHolder();

    /** @brief Construct Scintillator Part. */
    void ConstructScintillator();

    /** @brief Construct ZnS Part. */
    void ConstructZnS();

    /** @brief Construct PMT Glass Part. */
    void ConstructPMTGlass();

    /** @brief Construct TeflonOpticalProperties. */
    void CreateTeflonOpticalProperties();

    /** @brief Construct MylarOpticalProperties. */
    void CreateMylarOpticalProperties();

    /** @brief Construct DetectionOpticalProperties. */
    void CreateDetectionOpticalProperties();

    /** @brief Construct method required by Geant4 kernel. */
    G4VPhysicalVolume *Construct() override;

    /** @brief Print geometry parameters. */
    static void Print();
    ///@}

    /** @name Geometry Parameters */
    ///@{
    void SetScintillatorLength(const G4double Length) {
        fScintillatorLength = Length;
    };
    void SetScintillatorWidth(const G4double Width) {
        fScintillatorWidth = Width;
    };
    void SetScintillatorThickness(const G4double Thickness) {
        fScintillatorThickness = Thickness;
    };

    void SetZnSLength(const G4double Length) { fZnSLength = Length; };
    void SetZnSWidth(const G4double Width) { fZnSWidth = Width; };
    void SetZnSThickness(const G4double Thickness) {
        fZnSThickness = Thickness;
    };

    void SetDetectorDistance(const G4double Distance) {
        fDetectorDistance = Distance;
    };

    const float GetScintillatorLength() const { return fScintillatorLength; }
    const float GetScintillatorWidth() const { return fScintillatorWidth; }
    const float GetScintillatorThickness() const {
        return fScintillatorThickness;
    }
    const float GetZnSLength() const { return fZnSLength; }
    const float GetZnSWidth() const { return fZnSWidth; }
    const float GetZnSThickness() const { return fZnSThickness; }
    const float GetDetectorDistance() const { return fDetectorDistance; }
    ///@}

    /** @name Geometry Parameters */
    ///@{
    void SetScintillatorLY(const G4double LY) { fScintillatorLY = LY; };
    void SetZnSLY(const G4double LY) { fZnSLY = LY; };

    const float GetScintillatorLY() const { return fScintillatorLY; }
    const float GetZnSLY() const { return fZnSLY; }
    ///@}

  private:
    static const G4String path;

    /** @brief Geometry handler. */
    std::unique_ptr<Geometry> Geom;

    /** @brief UI messenger for runtime commands. */
    std::unique_ptr<OpticalSimulationGeometryMessenger> fGeometryMessenger;

    /** @brief Default materials. */
    G4Material *Vacuum;
    G4Material *Material;

    /** @brief Default magnetic and geometry values. */
    G4double fScintillatorLength = 0.1 * CLHEP::m;
    G4double fScintillatorWidth = 0.1 * CLHEP::m;
    G4double fScintillatorThickness = 0.1 * CLHEP::m;
    G4double fZnSLength = 0.1 * CLHEP::m;
    G4double fZnSWidth = 0.1 * CLHEP::m;
    G4double fZnSThickness = 0.1 * CLHEP::m;
    G4double fDetectorDistance = 0.1 * CLHEP::m;

    /** @brief Default Materials values. */
    G4double fScintillatorLY = 10000 / MeV;
    G4double fZnSLY = 44000 / MeV;

    /** @brief Visualization attributes (colors). */
    G4VisAttributes *invis = nullptr; // init all the pointers
    G4VisAttributes *white = nullptr;
    G4VisAttributes *gray = nullptr;
    G4VisAttributes *black = nullptr;
    G4VisAttributes *red = nullptr;
    G4VisAttributes *orange = nullptr;
    G4VisAttributes *yellow = nullptr;
    G4VisAttributes *green = nullptr;
    G4VisAttributes *cyan = nullptr;
    G4VisAttributes *blue = nullptr;
    G4VisAttributes *magenta = nullptr;

    /** @brief Logical volumes (geometry definitions). */
    G4LogicalVolume *LogicalWorld = nullptr;
    G4LogicalVolume *LogicalHolder = nullptr;
    G4LogicalVolume *LogicalPALLAS_ChambreDipole = nullptr;
    G4LogicalVolume *LogicalScintillator = nullptr;
    G4LogicalVolume *LogicalZnS = nullptr;
    G4LogicalVolume *LogicalPMTGlass = nullptr;
    G4LogicalVolume *LogicalPhotocathode = nullptr;

    /** @brief Physical volumes (placements in space). */
    G4VPhysicalVolume *PhysicalWorld = nullptr;
    G4VPhysicalVolume *PhysicalHolder = nullptr;
    G4VPhysicalVolume *PhysicalPALLAS_ChambreDipole = nullptr;
    G4VPhysicalVolume *PhysicalScintillator = nullptr;
    G4VPhysicalVolume *PhysicalZnS = nullptr;
    G4VPhysicalVolume *PhysicalPMTGlass = nullptr;
    G4VPhysicalVolume *PhysicalPhotocathode = nullptr;

    /** @brief Rotation matrices for component orientation. */
    G4RotationMatrix DontRotate;
    G4RotationMatrix Flip;
    G4RotationMatrix *RotationMatrix;
};
#endif
