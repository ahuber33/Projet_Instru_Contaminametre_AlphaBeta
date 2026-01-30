#include "OpticalSimulationGeometryMessenger.hh"

/**
 * @file OpticalSimulationGeometryMessenger.cc
 * @brief Implementation of the GeometryMessenger class for the
 * OpticalSimulation.
 *
 * This file defines the OpticalSimulationGeometryMessenger class, which
 * inherits from G4UImessenger. It provides an interface for the Geant4 UI to
 * control geometry and field parameters for the OpticalSimulation simulation.
 *
 * Responsibilities include:
 *  - Creating UI directories and commands for geometry, and materials
 *  - Setting geometry parameters such as ZnS & Scintillator dimensions
 *  - Passing user-specified values to the OpticalSimulationGeometryConstruction
 * class.
 *
 * Each UI command is linked to a setter function in the geometry construction
 * to update the simulation configuration dynamically.
 *
 * The messenger also provides functions to:
 *  - Retrieve current command values for UI display.
 *  - Delete all UI commands upon destruction to free memory.
 *
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Constructor
 * @param aDetector Pointer to the OpticalSimulation geometry construction class
 *
 * This constructor initializes all UI commands and directories for geometry,
 * display, and field control.
 */
OpticalSimulationGeometryMessenger::OpticalSimulationGeometryMessenger(
    OpticalSimulationGeometryConstruction *aDetector)
    : G4UImessenger(), fGeometry(aDetector) {
    // Create a main directory for all UI commands related to OpticalSimulation
    fExampleDir = new G4UIdirectory("/OpticalSimulation/");
    fExampleDir->SetGuidance("UI commands specific to this example");

    // Create sub-directory for geometry-related commands
    fGeometryDir = new G4UIdirectory("/OpticalSimulation/geometry/");
    fGeometryDir->SetGuidance("Geometry construction UI commands");

    // Create sub-directory for materials-related commands
    fMaterialsDir = new G4UIdirectory("/OpticalSimulation/materials/");
    fMaterialsDir->SetGuidance("Materials construction UI commands");

    // Command to print current geometry and field settings
    fPrintCmd = new G4UIcmdWithoutParameter("/OpticalSimulation/print", this);
    fPrintCmd->SetGuidance("Print current settings.");

    //=====================================
    // Geometry Commands
    //=====================================

    /**
     * @brief Command to set the Scintillator Length.
     *
     * Parameter: ScintillatorLength (double) unit
     */
    fGeometryScintillatorLengthCmd = new G4UIcmdWithADoubleAndUnit(
        "/OpticalSimulation/geometry/setScintillatorLength", this);
    fGeometryScintillatorLengthCmd->SetGuidance("Set Scintillator Length");
    fGeometryScintillatorLengthCmd->SetParameterName("ScintillatorLength",
                                                     false);
    fGeometryScintillatorLengthCmd->SetRange("ScintillatorLength>0.");
    fGeometryScintillatorLengthCmd->SetUnitCategory("Length");
    fGeometryScintillatorLengthCmd->AvailableForStates(G4State_PreInit,
                                                       G4State_Idle);
    fGeometryScintillatorLengthCmd->SetToBeBroadcasted(false);

    /**
     * @brief Command to set the Scintillator Width.
     *
     * Parameter: ScintillatorWidth (double) unit
     */
    fGeometryScintillatorWidthCmd = new G4UIcmdWithADoubleAndUnit(
        "/OpticalSimulation/geometry/setScintillatorWidth", this);
    fGeometryScintillatorWidthCmd->SetGuidance("Set Scintillator Width");
    fGeometryScintillatorWidthCmd->SetParameterName("ScintillatorWidth", false);
    fGeometryScintillatorWidthCmd->SetRange("ScintillatorWidth>0.");
    fGeometryScintillatorWidthCmd->SetUnitCategory("Length");
    fGeometryScintillatorWidthCmd->AvailableForStates(G4State_PreInit,
                                                      G4State_Idle);
    fGeometryScintillatorWidthCmd->SetToBeBroadcasted(false);

    /**
     * @brief Command to set the Scintillator Thickness.
     *
     * Parameter: ScintillatorThickness (double) unit
     */
    fGeometryScintillatorThicknessCmd = new G4UIcmdWithADoubleAndUnit(
        "/OpticalSimulation/geometry/setScintillatorThickness", this);
    fGeometryScintillatorThicknessCmd->SetGuidance(
        "Set Scintillator Thickness");
    fGeometryScintillatorThicknessCmd->SetParameterName("ScintillatorThickness",
                                                        false);
    fGeometryScintillatorThicknessCmd->SetRange("ScintillatorThickness>0.");
    fGeometryScintillatorThicknessCmd->SetUnitCategory("Length");
    fGeometryScintillatorThicknessCmd->AvailableForStates(G4State_PreInit,
                                                          G4State_Idle);
    fGeometryScintillatorThicknessCmd->SetToBeBroadcasted(false);

    /**
     * @brief Command to set the ZnS Length.
     *
     * Parameter: ZnSLength (double) unit
     */
    fGeometryZnSLengthCmd = new G4UIcmdWithADoubleAndUnit(
        "/OpticalSimulation/geometry/setZnSLength", this);
    fGeometryZnSLengthCmd->SetGuidance("Set ZnS Length");
    fGeometryZnSLengthCmd->SetParameterName("ZnSLength", false);
    fGeometryZnSLengthCmd->SetRange("ZnSLength>0.");
    fGeometryZnSLengthCmd->SetUnitCategory("Length");
    fGeometryZnSLengthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    fGeometryZnSLengthCmd->SetToBeBroadcasted(false);

    /**
     * @brief Command to set the ZnS Width.
     *
     * Parameter: ZnSWidth (double) unit
     */
    fGeometryZnSWidthCmd = new G4UIcmdWithADoubleAndUnit(
        "/OpticalSimulation/geometry/setZnSWidth", this);
    fGeometryZnSWidthCmd->SetGuidance("Set ZnS Width");
    fGeometryZnSWidthCmd->SetParameterName("ZnSWidth", false);
    fGeometryZnSWidthCmd->SetRange("ZnSWidth>0.");
    fGeometryZnSWidthCmd->SetUnitCategory("Length");
    fGeometryZnSWidthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    fGeometryZnSWidthCmd->SetToBeBroadcasted(false);

    /**
     * @brief Command to set the ZnS Thickness.
     *
     * Parameter: ZnSThickness (double) unit
     */
    fGeometryZnSThicknessCmd = new G4UIcmdWithADoubleAndUnit(
        "/OpticalSimulation/geometry/setZnSThickness", this);
    fGeometryZnSThicknessCmd->SetGuidance("Set ZnS Thickness");
    fGeometryZnSThicknessCmd->SetParameterName("ZnSThickness", false);
    fGeometryZnSThicknessCmd->SetRange("ZnSThickness>0.");
    fGeometryZnSThicknessCmd->SetUnitCategory("Length");
    fGeometryZnSThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    fGeometryZnSThicknessCmd->SetToBeBroadcasted(false);

    /**
     * @brief Command to set the Detector Distance.
     *
     * Parameter: DetectorDistance (double) unit
     */
    fGeometryDetectorDistanceCmd = new G4UIcmdWithADoubleAndUnit(
        "/OpticalSimulation/geometry/setDetectorDistance", this);
    fGeometryDetectorDistanceCmd->SetGuidance("Set Detector Distance");
    fGeometryDetectorDistanceCmd->SetParameterName("DetectorDistance", false);
    fGeometryDetectorDistanceCmd->SetRange("DetectorDistance>0.");
    fGeometryDetectorDistanceCmd->SetUnitCategory("Length");
    fGeometryDetectorDistanceCmd->AvailableForStates(G4State_PreInit,
                                                     G4State_Idle);
    fGeometryDetectorDistanceCmd->SetToBeBroadcasted(false);

    //=====================================
    // Materials Commands
    //=====================================

    /**
     * @brief Command to set the Scintillator EJ212 LY.
     *
     * Parameter: Lightyield EJ212 (double)
     */
    fGeometryScintillatorLYCmd = new G4UIcmdWithADouble(
        "/OpticalSimulation/materials/setScintillatorLY", this);
    fGeometryScintillatorLYCmd->SetGuidance(
        "Set Scintillator LY (photons/MeV)");
    fGeometryScintillatorLYCmd->SetParameterName("ScintillatorLY", false);
    fGeometryScintillatorLYCmd->SetRange("ScintillatorLY>0.");
    fGeometryScintillatorLYCmd->AvailableForStates(G4State_PreInit,
                                                   G4State_Idle);

    /**
     * @brief Command to set the ZnS:Ag LY.
     *
     * Parameter: Lightyield ZnS:Ag (double)
     */
    fGeometryZnSLYCmd =
        new G4UIcmdWithADouble("/OpticalSimulation/materials/setZnSLY", this);
    fGeometryZnSLYCmd->SetGuidance("Set Scintillator ZnS (photons/MeV)");
    fGeometryZnSLYCmd->SetParameterName("ZnSLY", false);
    fGeometryZnSLYCmd->SetRange("ZnSLY>0.");
    fGeometryZnSLYCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Destructor.
 *
 * Deletes all UI commands to free memory.
 */
OpticalSimulationGeometryMessenger::~OpticalSimulationGeometryMessenger() {
    delete fPrintCmd;
    delete fGeometryScintillatorLengthCmd;
    delete fGeometryScintillatorWidthCmd;
    delete fGeometryScintillatorThicknessCmd;
    delete fGeometryZnSLengthCmd;
    delete fGeometryZnSWidthCmd;
    delete fGeometryZnSThicknessCmd;
    delete fGeometryDetectorDistanceCmd;
    delete fGeometryScintillatorLYCmd;
    delete fGeometryZnSLYCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Apply a new value from the UI command.
 * @param aCommand Pointer to the UI command used.
 * @param aNewValue New value as string from the UI.
 *
 * Converts the string to the appropriate type and sets it in the
 * geometry construction.
 */
void OpticalSimulationGeometryMessenger::SetNewValue(G4UIcommand *aCommand,
                                                     G4String aNewValue) {
    if (aCommand == fPrintCmd) {
        fGeometry->Print();
    } else if (aCommand == fGeometryScintillatorLengthCmd) {
        fGeometry->SetScintillatorLength(
            fGeometryScintillatorLengthCmd->GetNewDoubleValue(aNewValue));
    } else if (aCommand == fGeometryScintillatorWidthCmd) {
        fGeometry->SetScintillatorWidth(
            fGeometryScintillatorWidthCmd->GetNewDoubleValue(aNewValue));
    } else if (aCommand == fGeometryScintillatorThicknessCmd) {
        fGeometry->SetScintillatorThickness(
            fGeometryScintillatorThicknessCmd->GetNewDoubleValue(aNewValue));
    } else if (aCommand == fGeometryZnSLengthCmd) {
        fGeometry->SetZnSLength(
            fGeometryZnSLengthCmd->GetNewDoubleValue(aNewValue));
    } else if (aCommand == fGeometryZnSWidthCmd) {
        fGeometry->SetZnSWidth(
            fGeometryZnSWidthCmd->GetNewDoubleValue(aNewValue));
    } else if (aCommand == fGeometryZnSThicknessCmd) {
        fGeometry->SetZnSThickness(
            fGeometryZnSThicknessCmd->GetNewDoubleValue(aNewValue));
    } else if (aCommand == fGeometryDetectorDistanceCmd) {
        fGeometry->SetDetectorDistance(
            fGeometryDetectorDistanceCmd->GetNewDoubleValue(aNewValue));
    } else if (aCommand == fGeometryScintillatorLYCmd) {
        fGeometry->SetScintillatorLY(
            fGeometryScintillatorLYCmd->GetNewDoubleValue(aNewValue));
    } else if (aCommand == fGeometryZnSLYCmd) {
        fGeometry->SetZnSLY(fGeometryZnSLYCmd->GetNewDoubleValue(aNewValue));
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Get the current value of a UI command.
 * @param aCommand Pointer to the UI command.
 * @return Current value of the command as a string.
 *
 * Retrieves the current value from the geometry construction and
 * converts it to a string for display in the UI.
 */
G4String
OpticalSimulationGeometryMessenger::GetCurrentValue(G4UIcommand *aCommand) {
    G4String cv;

    if (aCommand == fGeometryScintillatorLengthCmd) {
        cv = fGeometryScintillatorLengthCmd->ConvertToString(
            fGeometry->GetScintillatorLength(), "m");
    } else if (aCommand == fGeometryScintillatorWidthCmd) {
        cv = fGeometryScintillatorWidthCmd->ConvertToString(
            fGeometry->GetScintillatorWidth(), "m");
    } else if (aCommand == fGeometryScintillatorThicknessCmd) {
        cv = fGeometryScintillatorThicknessCmd->ConvertToString(
            fGeometry->GetScintillatorThickness(), "m");
    } else if (aCommand == fGeometryZnSLengthCmd) {
        cv = fGeometryZnSLengthCmd->ConvertToString(fGeometry->GetZnSLength(),
                                                    "m");
    } else if (aCommand == fGeometryZnSWidthCmd) {
        cv = fGeometryZnSWidthCmd->ConvertToString(fGeometry->GetZnSWidth(),
                                                   "m");
    } else if (aCommand == fGeometryZnSThicknessCmd) {
        cv = fGeometryZnSThicknessCmd->ConvertToString(
            fGeometry->GetZnSThickness(), "m");
    } else if (aCommand == fGeometryDetectorDistanceCmd) {
        cv = fGeometryDetectorDistanceCmd->ConvertToString(
            fGeometry->GetDetectorDistance(), "m");
    } else if (aCommand == fGeometryScintillatorLYCmd) {
        cv = fGeometryScintillatorLYCmd->ConvertToString(
            fGeometry->GetScintillatorLY());
    } else if (aCommand == fGeometryZnSLYCmd) {
        cv = fGeometryZnSLYCmd->ConvertToString(fGeometry->GetZnSLY());
    }
    return cv;
}
