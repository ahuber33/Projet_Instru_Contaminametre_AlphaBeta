#ifndef OpticalSimulationSteppingAction_h
#define OpticalSimulationSteppingAction_h

/**
 * @class OpticalSimulationSteppingAction
 * @brief Implements the Geant4 stepping action for the OpticalSimulation
 * project.
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 *
 * This class records tracking information at each step of a particle inside
 * the simulation. It extracts position, momentum, deposited energy and
 * other metadata (volume, process, etc.) for later analysis.
 *
 * It also handles quadrupole-related information and collimator updates.
 */

#include "G4GenericMessenger.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4UserSteppingAction.hh"
#include "OpticalSimulationRunAction.hh"

class OpticalSimulationSteppingAction : public G4UserSteppingAction {
  public:
    /**
     * @brief Constructor.
     *
     * Initializes the stepping action and messenger.
     */
    OpticalSimulationSteppingAction();

    /**
     * @brief Destructor.
     */
    ~OpticalSimulationSteppingAction() override;

    /**
     * @brief Store the initial particle information at the beginning of an
     * event.
     *
     * @param evtac Pointer to the event action where data will be stored.
     */
    void SetInputInformations(OpticalSimulationEventAction *evtac) const;
    void PrintOpticalProperties(const G4Step *);
    void CheckAndPrintSurface(const G4Step *);
    void CheckBoundaryStatus(const G4Step *, OpticalSimulationEventAction *);
    void CountScintillation(const G4Step *, OpticalSimulationEventAction *);
    void CountCerenkov(const G4Step *, OpticalSimulationEventAction *);
    void SetPhotonBirthInformation(const G4Step *,
                                   OpticalSimulationEventAction *);
    void SetInitialInformations(const G4Step *, OpticalSimulationEventAction *);

    /**
     * @brief Stepping action executed at each Geant4 step.
     *
     * Extracts step information (positions, momenta, energy deposition, etc.)
     * and stores it for later analysis. Also manages collimator updates.
     *
     * @param step Current Geant4 step.
     */
    void UserSteppingAction(const G4Step *step) override;

  private:
    /**
     * @struct StepPoint
     * @brief Helper structure to store spatial and momentum coordinates of a
     * step.
     */
    struct StepPoint {
        G4double x = 0.0;  ///< X position [mm]
        G4double y = 0.0;  ///< Y position [mm]
        G4double z = 0.0;  ///< Z position [mm]
        G4double px = 0.0; ///< X component of momentum direction
        G4double py = 0.0; ///< Y component of momentum direction
        G4double pz = 0.0; ///< Z component of momentum direction
    };

    // --- Configuration & control ---
    G4GenericMessenger *sMessenger =
        nullptr;                  ///< Command messenger for UI interaction
    G4bool TrackingStatus = true; ///< Enable/disable general tracking
    G4bool TrackingStatusCollimators =
        true; ///< Enable/disable collimator tracking

    // --- Track information ---
    G4Track *theTrack = nullptr; ///< Pointer to the current track
    G4String particleName;       ///< Particle name
    G4int particleID = 0;        ///< PDG particle ID
    G4int parentID = 0;          ///< Parent track ID
    G4int trackID = 0;           ///< Current track ID
    G4int stepNo = 0;            ///< Step number in the current track
    G4String endproc;            ///< Process at track end
    G4String volumeNamePreStep;  ///< Volume name at pre-step point
    G4String volumeNamePostStep; ///< Volume name at post-step point
    G4double angle = 0.0;
    G4double time = 0.0;

    // --- Energy ---
    G4double energy = 0.0;          ///< Kinetic energy [MeV]
    G4double energy_post = 0.0;     ///< Kinetic energy [MeV]
    G4double energyDeposited = 0.0; ///< Deposited energy [keV]

    // --- Step coordinates ---
    StepPoint preStep;  ///< Pre-step point (position & momentum)
    StepPoint postStep; ///< Post-step point (position & momentum)

    G4int VerbosityLevel = 0;
    G4bool PhotonTrackStatus = true;
};

#endif // OpticalSimulationSteppingAction_h
