/**
 * @file OpticalSimulationEventAction.cc
 * @brief Implementation of the EventAction class for the OpticalSimulation of
 * an alpha/beta contamineter.
 *
 * This file defines the OpticalSimulationEventAction class, which handles
 * per-event actions in the Geant4 simulation of the OpticalSimulation setup.
 *
 * The class works in conjunction with:
 *  - OpticalSimulationSteppingAction: for per-step updates within an event.
 *  - OpticalSimulationRunAction: to accumulate run-level statistics.
 *
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 */

#include "OpticalSimulationEventAction.hh" ///< Event action header
#include "OpticalSimulationRunAction.hh" ///< Run action header (for statistics accumulation)
#include "OpticalSimulationSteppingAction.hh" ///< Stepping action header (per-step updates)

/**
 * @brief Constructor for OpticalSimulationEventAction
 * @param suff Suffix used for naming ROOT branches or output files
 *
 * Initializes the event action and stores the provided suffix.
 */
OpticalSimulationEventAction::OpticalSimulationEventAction(const char *suff)
    : suffixe(suff) {}

/**
 * @brief Destructor for OpticalSimulationEventAction
 *
 * Cleans up any resources used by the event action.
 */
OpticalSimulationEventAction::~OpticalSimulationEventAction() {}

/**
 * @brief Called at the beginning of each event
 * @param evt Pointer to the current G4Event
 *
 * Resets all per-event statistics and counters to initial empty states.
 * This includes:
 * - Input statistics
 * - Optical statistics
 * - Zns Statistics
 * - Scintillator statistics
 */
void OpticalSimulationEventAction::BeginOfEventAction(const G4Event *evt) {
    /** Reset input statistics */
    StatsInput = {};
    StatsOptical = {};

    /** Reset Beam Stop (BS) and BSPEC YAG detector statistics */
    StatsZnS = {};
    StatsScintillator = {};
}

/**
 * @brief Called at the end of each event
 * @param evt Pointer to the current G4Event
 *
 * Updates run-level statistics by passing the per-event data to the
 * OpticalSimulationRunAction. Only non-empty or relevant data are updated
 * for input, BS YAG, and BSPEC YAG statistics, while quadrupole and
 * collimator statistics are always updated.
 */
void OpticalSimulationEventAction::EndOfEventAction(const G4Event *evt) {
    /** Get pointer to current run action */
    OpticalSimulationRunAction *runac =
        (OpticalSimulationRunAction *)(G4RunManager::GetRunManager()
                                           ->GetUserRunAction());

    /** Update input energy statistics if valid */
    if (StatsInput.energy > 0)
        runac->UpdateStatisticsInput(StatsInput);

    /** Update Beam Stop YAG statistics if not empty */
    if (!StatsZnS.energy.empty())
        runac->UpdateStatisticsZnS(StatsZnS);

    /** Update Beam Stop SPEC YAG statistics if not empty */
    if (!StatsScintillator.energy.empty())
        runac->UpdateStatisticsScintillator(StatsScintillator);

    if (StatsOptical.ScintillationSc < 0) {
        StatsOptical.IncidentE = StatsInput.energy;
        StatsOptical.DepositTotal = StatsScintillator.deposited_energy_event +
                                    StatsZnS.deposited_energy_event;
        StatsOptical.DepositSc = StatsScintillator.deposited_energy_event;
        StatsOptical.DepositZnS = StatsZnS.deposited_energy_event;
        StatsOptical.GeneratedSc =
            StatsOptical.ScintillationSc + StatsOptical.CerenkovSc;
        StatsOptical.GeneratedZnS =
            StatsOptical.ScintillationZnS + StatsOptical.CerenkovZnS;
        StatsOptical.GeneratedTotal =
            StatsOptical.GeneratedSc + StatsOptical.GeneratedZnS;

        StatsOptical.BulkAbsTotal =
            StatsOptical.BulkAbsSc + StatsOptical.BulkAbsZnS;

        float Absfrac =
            100 * StatsOptical.Absorbed / StatsOptical.GeneratedTotal;
        float BulkfracZnS =
            100 * StatsOptical.BulkAbsZnS / StatsOptical.GeneratedTotal;
        float BulkfracSc =
            100 * StatsOptical.BulkAbsSc / StatsOptical.GeneratedTotal;
        float BulkfracTotal = BulkfracZnS + BulkfracSc;
        float Escfrac =
            100 * StatsOptical.Escaped / StatsOptical.GeneratedTotal;
        float Failfrac =
            100 * StatsOptical.Failed / StatsOptical.GeneratedTotal;
        float Killedfrac =
            100 * StatsOptical.Killed / StatsOptical.GeneratedTotal;

        // Output the results
        G4cout << "\n\nRun "
               << G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID()
               << " >>> Event " << evt->GetEventID() << G4endl;
        G4cout << "Incident Energy :                    "
               << StatsOptical.IncidentE / keV << " keV " << G4endl;
        G4cout << "Energy Deposited TOTAL :             "
               << StatsOptical.DepositTotal << " keV " << G4endl;
        G4cout << "     Energy Deposited ZnS :          "
               << StatsOptical.DepositZnS << " keV " << G4endl;
        G4cout << "     Energy Deposited Sc :           "
               << StatsOptical.DepositSc << " keV " << G4endl;
        G4cout << "Photons Generated TOTAL :            "
               << StatsOptical.GeneratedTotal << G4endl;
        G4cout << "     Photons Generated Zns :         "
               << StatsOptical.GeneratedZnS << G4endl;
        G4cout << "         Scintillation :             "
               << StatsOptical.ScintillationZnS << G4endl;
        G4cout << "         Cerenkov :                  "
               << StatsOptical.CerenkovZnS << G4endl;
        G4cout << "     Photons Generated Sc :          "
               << StatsOptical.GeneratedSc << G4endl;
        G4cout << "         Scintillation :             "
               << StatsOptical.ScintillationSc << G4endl;
        G4cout << "         Cerenkov :                  "
               << StatsOptical.CerenkovSc << G4endl;

        G4cout << "\nPhotons Surface Absorbed :           "
               << StatsOptical.Absorbed << "        " << Absfrac << " % "
               << G4endl;
        G4cout << "Photons Bulk Absorbed Total :        "
               << StatsOptical.BulkAbsTotal << "        " << BulkfracTotal
               << " % " << G4endl;
        G4cout << "     Photons Bulk Absorbed ZnS :     "
               << StatsOptical.BulkAbsZnS << "        " << BulkfracZnS << " % "
               << G4endl;
        G4cout << "     Photons Bulk Absorbed Sc :      "
               << StatsOptical.BulkAbsSc << "        " << BulkfracSc << " % "
               << G4endl;
        G4cout << "Photons Escaped:                     "
               << StatsOptical.Escaped << "        " << Escfrac << " % "
               << G4endl;
        G4cout << "Photons Transmitted to PMT:          " << StatsOptical.Failed
               << "        " << Failfrac << " % " << G4endl;
        G4cout << "Photons Collected in PMT (QE):       "
               << StatsOptical.Detected << G4endl;
        G4cout << "Photons Killed by user:              " << StatsOptical.Killed
               << G4endl;
        G4cout
            << "Total Photons Considered:            "
            << StatsOptical.Absorbed + StatsOptical.BulkAbsTotal +
                   StatsOptical.Escaped + StatsOptical.Failed +
                   StatsOptical.Detected + StatsOptical.Killed
            << "        "
            //<< Absfrac + Bulkfrac + Escfrac + Failfrac + efficiency << " % "
            << G4endl;
        G4cout << "" << G4endl;
    }
    runac->UpdateStatisticsOptical(StatsOptical);
}
