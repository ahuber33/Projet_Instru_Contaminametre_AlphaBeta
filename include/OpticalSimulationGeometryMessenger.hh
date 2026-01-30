#ifndef OpticalSimulationGeometryMessenger_H
#define OpticalSimulationGeometryMessenger_H

/**
 * @class OpticalSimulationGeometryMessenger
 * @brief UI messenger for the Geometry construction in optical simulation
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 *
 * Provides UI commands to setup detector and readout geometry (prior to
 * initialization). Length, distance, gradients and display can be changed.
 */

#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh" // for G4UIcmdWithADoubleAndUnit
#include "G4UIcmdWithAnInteger.hh"      // for G4UIcmdWithAnInteger
#include "G4UIcmdWithoutParameter.hh"   // for G4UIcmdWithoutParameter
#include "G4UIdirectory.hh"             // for G4UIdirectory
#include "OpticalSimulationGeometryConstruction.hh" // for OpticalSimulationGeometryConstruction

class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;
class G4UIdirectory;
class OpticalSimulationGeometryConstruction;

class OpticalSimulationGeometryMessenger : public G4UImessenger {
  public:
    OpticalSimulationGeometryMessenger(OpticalSimulationGeometryConstruction *);
    ~OpticalSimulationGeometryMessenger();

    /// Invokes appropriate methods based on the typed command
    virtual void SetNewValue(G4UIcommand *, G4String) final;
    /// Retrieves the current settings
    virtual G4String GetCurrentValue(G4UIcommand *) final;

  private:
    /// Detector construction to setup
    OpticalSimulationGeometryConstruction *fGeometry = nullptr;
    /// Command to set the directory common to all messengers in this example
    /// /Par04
    G4UIdirectory *fExampleDir = nullptr;
    /// Command to set the directory for detector settings
    /// /OpticalSimulation/geometry
    G4UIdirectory *fGeometryDir = nullptr;
    /// /OpticalSimulation/materials
    G4UIdirectory *fMaterialsDir = nullptr;
    /// Command printing current settings
    G4UIcmdWithoutParameter *fPrintCmd;

    /// GEOMETRY
    ///  Command to set the Scintillator Length
    G4UIcmdWithADoubleAndUnit *fGeometryScintillatorLengthCmd = nullptr;
    /// Command to set the Scintillator Width
    G4UIcmdWithADoubleAndUnit *fGeometryScintillatorWidthCmd = nullptr;
    /// Command to set the Scintillator Thickness
    G4UIcmdWithADoubleAndUnit *fGeometryScintillatorThicknessCmd = nullptr;
    /// Command to set the Scintillator Length
    G4UIcmdWithADoubleAndUnit *fGeometryZnSLengthCmd = nullptr;
    /// Command to set the Scintillator Width
    G4UIcmdWithADoubleAndUnit *fGeometryZnSWidthCmd = nullptr;
    /// Command to set the Scintillator Thickness
    G4UIcmdWithADoubleAndUnit *fGeometryZnSThicknessCmd = nullptr;
    /// Command to set the Detector Distance
    G4UIcmdWithADoubleAndUnit *fGeometryDetectorDistanceCmd = nullptr;

    /// MATERIALS
    ///  Command to set the Scintillator LY
    G4UIcmdWithADouble *fGeometryScintillatorLYCmd = nullptr;
    ///  Command to set the ZnS LY
    G4UIcmdWithADouble *fGeometryZnSLYCmd = nullptr;
};

#endif
