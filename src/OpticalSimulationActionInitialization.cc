/**
 * @file OpticalSimulationActionInitialization.cc
 * @brief Implementation of the ActionInitialization class for the Optical
 * simulation.
 *
 * This file defines the OpticalSimulationActionInitialization class, which is
 * responsible for setting up all user actions required for a Geant4 simulation
 * of the OpticalSimulation. It manages the creation and assignment of the
 * following actions:
 *   - Primary generator action
 *   - Run action
 *   - Event action
 *   - Stepping action
 *
 * In multithreaded mode, this class also defines master-thread-specific actions
 * such as RunAction. It stores configuration parameters such as the number of
 * events, number of threads, and output suffix, and provides access to the
 * simulation geometry.
 *
 * This class inherits from G4VUserActionInitialization and overrides Build()
 * and BuildForMaster() to provide thread-aware initialization of user actions.
 *
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 */

#include "OpticalSimulationActionInitialization.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Constructor for ActionInitialization class
 * @param suff Suffix string to append to output files
 * @param N Number of events to generate
 * @param Ncores Number of threads to use in multithreaded mode
 * @param pMT Flag indicating whether multithreading is enabled
 * @param geometry Pointer to the simulation geometry
 */
OpticalSimulationActionInitialization::OpticalSimulationActionInitialization(
    const char *suff, size_t N, size_t Ncores, G4bool pMT,
    OpticalSimulationGeometryConstruction *geometry)
    : G4VUserActionInitialization(), suffixe(suff), NEventsGenerated(N),
      numThreads(Ncores), flag_MT(pMT), fGeometry(geometry) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Destructor for ActionInitialization class
 */
OpticalSimulationActionInitialization::
    ~OpticalSimulationActionInitialization() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Build actions for the master thread
 *
 * This function is called in multithreaded mode to define actions
 * that are executed only in the master thread, such as RunAction.
 */
void OpticalSimulationActionInitialization::BuildForMaster() const {
    SetUserAction(
        new OpticalSimulationRunAction(suffixe, NEventsGenerated, flag_MT));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Build actions for worker threads or single-threaded runs
 *
 * This function creates and assigns all user actions required for the
 * simulation:
 * - PrimaryGeneratorAction
 * - RunAction
 * - EventAction
 * - SteppingAction
 */
void OpticalSimulationActionInitialization::Build() const {
    // Create primary generator action
    auto *generator = new OpticalSimulationPrimaryGeneratorAction(
        NEventsGenerated, numThreads, flag_MT);

    // Create run action
    auto *runAction =
        new OpticalSimulationRunAction(suffixe, NEventsGenerated, flag_MT);

    // Create event action
    auto *eventAction = new OpticalSimulationEventAction(suffixe);

    // Provide the run action with pointers to the primary generator and
    // geometry
    runAction->SetPrimaryGenerator(generator);
    runAction->SetGeometry(fGeometry);

    // Assign user actions to the simulation
    SetUserAction(generator);
    SetUserAction(runAction);
    SetUserAction(eventAction);
    SetUserAction(new OpticalSimulationSteppingAction());
}
