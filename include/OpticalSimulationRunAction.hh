#ifndef OpticalSimulationRunAction_h
#define OpticalSimulationRunAction_h 1

/**
 * @class OpticalSimulationRunAction
 * @brief Defines the run-level actions and statistics handling for the PALLAS
 * simulation.
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 *
 * This header declares the `OpticalSimulationRunAction` class, which inherits
 * from Geant4's `G4UserRunAction` to manage actions at the beginning and end of
 * each run. It handles:
 *  - Collection and storage of run-wide statistics
 *  - ROOT file and tree creation for data output
 *  - Synchronization in multithreaded runs
 *  - Coordination with primary generator and geometry configuration
 *
 *
 * Data recorded here typically includes:
 *  - Initial parameters
 *  - EM Informations for ZnS & Scintillator
 *  - Optical Informations for ZnS & Scintillator
 *
 * @note This class interacts closely with:
 *       - `OpticalSimulationPrimaryGeneratorAction`
 *       - `OpticalSimulationGeometryConstruction`
 */

// Include base classes and Geant4 utilities
#include "G4Run.hh" // Run object for event accumulation
#include "G4RunManager.hh"
#include "G4UImanager.hh"     // UI manager (for commands)
#include "G4UserRunAction.hh" // Base class for user-defined run actions
#include "G4VVisManager.hh"   // Visualization manager
#include "OpticalSimulationEventAction.hh"
#include "OpticalSimulationGeometryConstruction.hh"
#include "OpticalSimulationPrimaryGeneratorAction.hh"
#include "TBranch.h"
#include "TFile.h" // ROOT file I/O
#include "TTree.h"
#include <mutex>

// Forward declarations
class OpticalSimulationPrimaryGeneratorAction;
class OpticalSimulationGeometryConstruction;

/**
 * @class OpticalSimulationRunAction
 * @brief Manages run-level actions, statistics, and ROOT output.
 */
class OpticalSimulationRunAction : public G4UserRunAction {
  public:
    /**
     * @brief Constructor
     * @param fileName Base name for ROOT output file
     * @param nEventsGenerated Number of events to process
     * @param multithreaded True if running with multithreading
     */
    OpticalSimulationRunAction(const char *fileName, size_t nEventsGenerated,
                               G4bool multithreaded);

    /// Destructor
    ~OpticalSimulationRunAction();

    /// Called at the start of each run
    void BeginOfRunAction(const G4Run *run) override;

    /// Called at the end of each run
    void EndOfRunAction(const G4Run *run) override;

    /// Generic template to update cumulative statistics in a ROOT tree
    template <typename T>
    static void UpdateStatistics(T &stats, const T &newStats, TTree *tree);

    // --- Specific statistics update methods ---
    void UpdateStatisticsInput(RunTallyInput);
    void UpdateStatisticsZnS(RunTallySc);
    void UpdateStatisticsScintillator(RunTallySc);
    void UpdateStatisticsOptical(RunTallyOptical);

    /// Set the primary generator reference
    void SetPrimaryGenerator(OpticalSimulationPrimaryGeneratorAction *gen);

    /// Set the geometry reference
    void SetGeometry(OpticalSimulationGeometryConstruction *geom);

  private:
    // --- Output configuration ---
    G4String suffixe;  ///< File suffix for ROOT outputs
    G4String fileName; ///< Base file name for ROOT outputs

    // --- Stored run statistics ---
    RunTallyInput StatsInput;
    RunTallySc StatsZnS;
    RunTallySc StatsScintillator;
    RunTallyOptical StatsOptical;

    size_t NEventsGenerated; ///< Number of events generated in the run
    G4bool flag_MT;          ///< Multithreading enabled flag

    // --- ROOT file and trees ---
    TFile *f = nullptr;
    TTree *Tree_Input = nullptr;
    TTree *Tree_ZnS = nullptr;
    TTree *Tree_Scintillator = nullptr;
    TTree *Tree_Optical = nullptr;
    TBranch *RunBranch = nullptr;

    time_t start; ///< Start time of the run

    // --- Thread-safety ---
    static std::atomic<int> activeThreads;
    static G4Mutex fileMutex;

  protected:
    OpticalSimulationPrimaryGeneratorAction *fPrimaryGenerator =
        nullptr; ///< Primary generator reference
    OpticalSimulationGeometryConstruction *fGeometry =
        nullptr; ///< Geometry reference
};

#endif // OpticalSimulationRunAction_h