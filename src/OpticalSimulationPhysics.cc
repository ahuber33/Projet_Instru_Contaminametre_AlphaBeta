/**
 * @file OpticalSimulationPhysics.cc
 * @brief Implementation of the OpticalSimulationPhysics custom physics list.
 *
 * This file implements the `OpticalSimulationPhysics` class, a custom physics
 * list derived from Geant4's `G4VModularPhysicsList`. It defines the set of
 * physics processes to be used in the Optical simulation, tailored for
 * both electromagnetic and hadronic interactions, including ions, neutrons, and
 * radioactive decays.
 *
 * The physics list includes:
 *  - High-precision hadronic elastic and inelastic processes (neutrons and
 * ions)
 *  - Ion-specific elastic and inelastic physics
 *  - Stopping physics for particles coming to rest
 *  - Gamma-nuclear interactions
 *  - Electromagnetic physics with high-precision settings
 *  - Decay and radioactive decay processes
 *
 * The class also configures the nuclide table to store unstable isotopes with
 * half-lives above a threshold and sets default production cuts for secondary
 * particles.
 *
 * Usage:
 *  - Instantiate `OpticalSimulationPhysics` and set it as the physics list in
 *    the Geant4 run manager.
 *
 * @note The constructor automatically registers all physics modules.
 *
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 */

#include "OpticalSimulationPhysics.hh"

// ============================================================
// Constructor
// ============================================================
/**
 * @brief Constructs the custom physics list and registers the desired physics
 * modules.
 *
 * The constructor:
 *  - Sets the verbosity level for physics processes.
 *  - Configures the nuclide table for radioactive decay handling.
 *  - Registers high-precision hadronic physics for neutrons and ions.
 *  - Adds electromagnetic physics with high-accuracy settings.
 *  - Includes decay and radioactive decay processes.
 */
OpticalSimulationPhysics::OpticalSimulationPhysics() {
    // Verbosity level for physics processes
    G4int verb = 1;
    SetVerboseLevel(verb);

    // --- Nuclide Table configuration ---
    // Set half-life threshold to 1 ns for storing unstable isotopes
    const G4double meanLife = 1 * CLHEP::nanosecond;
    const G4double halfLife = meanLife * std::log(2);
    G4NuclideTable::GetInstance()->SetThresholdOfHalfLife(halfLife);

    // =============================
    // Physics Modules Registration
    // =============================

    // --- Hadron Elastic Scattering ---
    // RegisterPhysics(new G4HadronElasticPhysicsHP(
    //     verb)); ///< High-precision elastic scattering for low-energy
    //     neutrons

    // --- Hadron Inelastic Physics ---
    // RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP(
    //     verb)); ///< Binary Cascade model + HP neutron model

    // --- Ion Elastic Scattering ---
    RegisterPhysics(new G4IonElasticPhysics(verb));

    // --- Ion Inelastic Physics ---
    RegisterPhysics(new G4IonPhysicsXS(
        verb)); ///< Uses cross-section data for ion interactions

    // --- Stopping Physics ---
    RegisterPhysics(
        new G4StoppingPhysics(verb)); ///< Handles particles coming to rest

    // --- Gamma-Nuclear Physics ---
    // RegisterPhysics(
    //     new G4EmExtraPhysics()); ///< Includes gamma-nuclear interactions

    // --- Electromagnetic Physics ---
    RegisterPhysics(
        new G4EmStandardPhysics_option3()); ///< High-precision EM physics

    // --- Decay Processes ---
    RegisterPhysics(new G4DecayPhysics());

    // --- Radioactive Decay ---
    RegisterPhysics(new G4RadioactiveDecayPhysics());

    // Optical Physics
    auto opticalParams = G4OpticalParameters::Instance();
    // Désactiver uniquement Cerenkov
    opticalParams->SetProcessActivation("Cerenkov", false);

    // Laisser Scintillation active (par défaut true)
    // opticalParams->SetProcessActivation("Scintillation", true);
    // opticalParams->SetVerboseLevel(2);

    RegisterPhysics(new G4OpticalPhysics());
}

// ============================================================
// Destructor
// ============================================================
/**
 * @brief Destructor (no manual cleanup required, Geant4 handles physics
 * constructors).
 */
OpticalSimulationPhysics::~OpticalSimulationPhysics() {}
