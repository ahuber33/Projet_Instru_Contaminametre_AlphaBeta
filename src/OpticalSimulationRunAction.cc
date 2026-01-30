/**
 * @file OpticalSimulationRunAction.cc
 * @brief Implements run-level setup, data collection, and output handling for
 * the Optical simulation.
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 *
 * This file contains the method definitions for the
 * `OpticalSimulationRunAction` class declared in
 * `OpticalSimulationRunAction.hh`. It manages:
 *  - Initialization of run-wide ROOT files and trees
 *  - Thread-safe data collection in multi-threaded runs
 *  - Branch creation for all recorded statistics
 *  - Interaction with primary generator and geometry to populate run metadata
 *  - Begin/end-of-run hooks to prepare and finalize data storage
 *
 * The run action workflow:
 *  - **BeginOfRunAction**:
 *      - Locks file access (multi-thread safety)
 *      - Builds the ROOT output file name based on threading context
 *      - Creates TTree objects for each statistics category
 *      - Defines ROOT branches for run-wide parameters and measurements
 *      - Initializes the random seed
 *  - **During the run**:
 *      - Updates statistics via `UpdateStatistics()` and specialized variants
 *  - **EndOfRunAction**:
 *      - Finalizes statistics
 *      - Writes all TTrees to the ROOT file
 *      - Closes the file and releases resources
 *
 * Thread safety is ensured via:
 *  - `std::atomic<int> activeThreads` for counting active threads
 *  - `G4Mutex fileMutex` for synchronized file access
 *
 * @note This class uses Geant4's ROOT integration to structure physics
 *       output in an analysis-friendly format.
 */

// Include class header
#include "OpticalSimulationRunAction.hh"

// --- Static member initialization ---
std::atomic<int> OpticalSimulationRunAction::activeThreads(
    0); ///< Counter for active threads
G4Mutex OpticalSimulationRunAction::fileMutex =
    G4MUTEX_INITIALIZER; ///< Mutex for file protection

// --- Constructor ---
OpticalSimulationRunAction::OpticalSimulationRunAction(const char *suff,
                                                       size_t N, G4bool pMT)
    : suffixe(suff), NEventsGenerated(N), flag_MT(pMT) {}

// --- Destructor ---
OpticalSimulationRunAction::~OpticalSimulationRunAction() {}

// --- Primary generator reference setter ---
void OpticalSimulationRunAction::SetPrimaryGenerator(
    OpticalSimulationPrimaryGeneratorAction *gen) {
    fPrimaryGenerator = gen;
}

// --- Geometry reference setter ---
void OpticalSimulationRunAction::SetGeometry(
    OpticalSimulationGeometryConstruction *geom) {
    fGeometry = geom;
}

/**
 * @brief Utility template to create simple branches in a TTree.
 * @tparam T Data type (int, float, etc.)
 * @param tree Pointer to the TTree to populate
 * @param branches List of name-pointer pairs
 */
template <typename T>
static void
CreateBranches(TTree *tree,
               const std::vector<std::pair<const char *, T *>> &branches) {
    for (const auto &b : branches) {
        tree->Branch(b.first, b.second,
                     (std::string(b.first) + "/" +
                      (std::is_same<T, int>::value     ? "I"
                       : std::is_same<T, float>::value ? "F"
                                                       : ""))
                         .c_str());
    }
}

/**
 * @brief Creates ROOT branches specific to YAG detector statistics.
 * @param tree ROOT tree to populate
 * @param stats Sc statistics structure
 */
static void CreateScBranches(TTree *tree, RunTallySc &stats) {
    tree->Branch("x_entrance", "vector<float>", &stats.x_entrance);
    tree->Branch("y_entrance", "vector<float>", &stats.y_entrance);
    tree->Branch("z_entrance", "vector<float>", &stats.z_entrance);
    tree->Branch("parentID", "vector<int>", &stats.parentID);
    tree->Branch("particleID", "vector<int>", &stats.particleID);
    tree->Branch("energy", "vector<float>", &stats.energy);
    tree->Branch("deposited_energy", "vector<float>",
                 &stats.total_deposited_energy);
    tree->Branch("deposited_energy_event", &stats.deposited_energy_event,
                 "deposited_energy_event/F");
}

/**
 * @brief Creates ROOT branches specific to Opticalphoton statistics.
 * @param tree ROOT tree to populate
 * @param stats Optical statistics structure
 */
static void CreateOpticalBranches(TTree *tree, RunTallyOptical &stats) {
    tree->Branch("incident_E", &stats.IncidentE, "incident_E/F");
    tree->Branch("deposit_ZnS", &stats.DepositZnS, "deposit_ZnS/F");
    tree->Branch("deposit_Sc", &stats.DepositSc, "deposit_Sc/F");
    tree->Branch("scintillation_ZnS", &stats.ScintillationZnS,
                 "scintillation_ZnS/I");
    tree->Branch("scintillation_Sc", &stats.ScintillationSc,
                 "scintillation_Sc/I");
    tree->Branch("cerenkov_ZnS", &stats.CerenkovZnS, "cerenkov_ZnS/I");
    tree->Branch("cerenkov_Sc", &stats.CerenkovSc, "cerenkov_Sc/I");
    tree->Branch("bulk_abs_ZnS", &stats.BulkAbsZnS, "bulk_abs_ZnS/I");
    tree->Branch("bulk_abs_Sc", &stats.BulkAbsSc, "bulk_abs_Sc/I");
    tree->Branch("absorbed", &stats.Absorbed, "absorbed/I");
    tree->Branch("escaped", &stats.Escaped, "escaped/I");
    tree->Branch("failed", &stats.Failed, "failed/I");
    tree->Branch("killed", &stats.Killed, "killed/I");
    tree->Branch("detected", &stats.Detected, "detected/I");
    // tree->Branch("exit_light_position_x", "vector<float>",
    //              &stats.ExitLightPositionX);
    // tree->Branch("exit_light_position_y", "vector<float>",
    //              &stats.ExitLightPositionY);
    // tree->Branch("exit_light_position_z", "vector<float>",
    //              &stats.ExitLightPositionZ);
    tree->Branch("detector_position_x", "vector<float>",
                 &stats.DetectorPositionX);
    tree->Branch("detector_position_y", "vector<float>",
                 &stats.DetectorPositionY);
    tree->Branch("detector_position_z", "vector<float>",
                 &stats.DetectorPositionZ);
    tree->Branch("birth_wavelength", "vector<float>", &stats.BirthWavelength);
    tree->Branch("birth_wavelength_detected", "vector<float>",
                 &stats.BirthWavelengthDetected);
    tree->Branch("time", "vector<float>", &stats.Time);
    // tree->Branch("rayleigh", "vector<int>", &stats.Rayleigh);
    // tree->Branch("total_reflections", "vector<int>",
    // &stats.Total_Reflections); tree->Branch("wrap_reflections",
    // "vector<int>", &stats.Wrap_Reflections); tree->Branch("total_length",
    // "vector<float>", &stats.TotalLength);
    tree->Branch("angle_creation", "vector<float>", &stats.Angle_creation);
    tree->Branch("angle_detection", "vector<float>", &stats.Angle_detection);
    // tree->Branch("final_state", "vector<int>", &stats.FinalState);
}

//---------------------------------------------------------
//  Generic statistics update function
//---------------------------------------------------------
/**
 * @brief Thread-safe update of statistics and ROOT tree filling.
 * @tparam T Type of the statistics structure
 * @param stats Destination statistics object (persistent in the run)
 * @param newStats Source statistics (new values)
 * @param tree ROOT tree to fill
 */
template <typename T>
void OpticalSimulationRunAction::UpdateStatistics(T &stats, const T &newStats,
                                                  TTree *tree) {
    std::lock_guard<std::mutex> lock(fileMutex);
    stats = newStats;
    if (tree)
        tree->Fill();
    else
        G4cerr << "Error: Tree is nullptr" << G4endl;
}

// --- Specific statistics update wrappers ---
void OpticalSimulationRunAction::UpdateStatisticsInput(RunTallyInput a) {
    UpdateStatistics(StatsInput, a, Tree_Input);
}
void OpticalSimulationRunAction::UpdateStatisticsZnS(RunTallySc a) {
    UpdateStatistics(StatsZnS, a, Tree_ZnS);
}
void OpticalSimulationRunAction::UpdateStatisticsScintillator(RunTallySc a) {
    UpdateStatistics(StatsScintillator, a, Tree_Scintillator);
}
void OpticalSimulationRunAction::UpdateStatisticsOptical(RunTallyOptical a) {
    UpdateStatistics(StatsOptical, a, Tree_Optical);
}

//-----------------------------------------------------
//  BeginOfRunAction
//-----------------------------------------------------
/**
 * @brief Called at the start of each run to set up ROOT output structures and
 * initialize state.
 * @param aRun Pointer to the current G4Run
 */
void OpticalSimulationRunAction::BeginOfRunAction(const G4Run *aRun) {
    // Populate branches for each TTree...
    G4AutoLock lock(&fileMutex); // Automatic mutex lock

    start = time(NULL); // start the timer clock to calculate run times

    int a = activeThreads;

    std::string s = flag_MT ? "_" + std::to_string(activeThreads) : "";
    fileName = suffixe + s + ".root";

    G4cout << "Filename = " << fileName << G4endl;

    f = new TFile(fileName.c_str(), "RECREATE");

    // Creating trees for different types of run information
    Tree_Input = new TTree(
        "Input", "Input Information"); // Tree to access Input information
    Tree_ZnS = new TTree("ZnS", "ZnS Information"); // Tree to access ZnS infos
    Tree_Scintillator = new TTree(
        "Scintillator",
        "Scintillator Information"); // Tree to access Scintillator infos
    Tree_Optical = new TTree(
        "Optical", "Optical Information"); // Tree to access Optical information

    //**********INFORMATIONS FROM THE INPUT***************************
    std::vector<std::pair<const char *, float *>> inputBranches = {
        {"x", &StatsInput.x},          {"xp", &StatsInput.xp},
        {"y", &StatsInput.y},          {"yp", &StatsInput.yp},
        {"z", &StatsInput.z},          {"zp", &StatsInput.zp},
        {"energy", &StatsInput.energy}};
    CreateBranches(Tree_Input, inputBranches);

    //************************************INFORMATIONS FROM THE
    // YAGs*****************************************
    //************************************INFORMATIONS FROM THE BS
    // YAG*****************************************
    CreateScBranches(Tree_ZnS, StatsZnS);

    //************************************INFORMATIONS FROM THE BSPEC
    // YAG*****************************************
    CreateScBranches(Tree_Scintillator, StatsScintillator);

    //************************************INFORMATIONS FROM THE OPTICAL
    // PHOTON*****************************************
    CreateOpticalBranches(Tree_Optical, StatsOptical);

    // set the random seed to the CPU clock
    // G4Random::setTheEngine(new CLHEP::HepJamesRandom);
    G4long seed = time(NULL) + a;
    G4Random::setTheSeed(seed);
    // G4Random::setTheSeed(1712670533);
    G4cout << "seed = " << seed << G4endl;

    G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

    if (G4VVisManager::GetConcreteInstance()) {
        G4UImanager *UI = G4UImanager::GetUIpointer();
        UI->ApplyCommand("/vis/scene/notifyHandlers");
    }

    activeThreads++;
}

//-----------------------------------------------------
//  EndOfRunAction
//-----------------------------------------------------
/**
 * @brief Called at the end of each run to write collected data to the ROOT file
 * and clean up.
 * @param aRun Pointer to the current G4Run
 */
void OpticalSimulationRunAction::EndOfRunAction(const G4Run *aRun) {
    G4AutoLock lock(&fileMutex);

    // Write all trees to ROOT file
    f->cd();
    Tree_Input->Write();
    Tree_ZnS->Write();
    Tree_Scintillator->Write();
    Tree_Optical->Write();
    f->Close();
    delete f;
    f = nullptr;

    if (G4VVisManager::GetConcreteInstance())
        G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");

    G4cout << "Leaving Run Action" << G4endl;
}
