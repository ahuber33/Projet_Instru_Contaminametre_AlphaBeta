#ifndef OpticalSimulationPhysics_h
#define OpticalSimulationPhysics_h 1

/**
 * @class OpticalSimulationPhysics
 * @brief Custom physics list for the OpticalSimulation project.
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 * This class defines a modular physics list for Geant4 simulations
 * in the Opticalsimulaiton project. It configures a combination of
 * electromagnetic, hadronic, ion, and decay physics, using high-precision (HP)
 * models where relevant for accurate plasma and nuclear interaction modeling.
 *
 * Key features:
 * - Uses HP models for neutron elastic and inelastic scattering
 * - Includes EM Option3 physics for improved multiple scattering accuracy
 * - Registers radioactive decay physics for isotope handling
 * - Configures nuclide table thresholds for short-lived isotopes
 *
 * @note This physics list is optimized for applications involving
 *      plasma physics and detailed nuclear interactions.
 */

// =============================
// Geant4 Base Class
// =============================
#include "G4ProcessManager.hh"
#include "G4VModularPhysicsList.hh"

// =============================
// Units
// =============================
#include "CLHEP/Units/SystemOfUnits.h"

// =============================
// Nuclide Table (for radioactive decay threshold setup)
// =============================
#include "G4NuclideTable.hh"

// =============================
// Physics Modules
// =============================

// --- Decay Physics ---
#include "G4DecayPhysics.hh"            ///< Handles particle decays
#include "G4RadioactiveDecayPhysics.hh" ///< Handles radioactive isotope decays

// --- Electromagnetic Physics ---
#include "G4EmExtraPhysics.hh" ///< Extra EM processes (gamma-nuclear, synchrotron, etc.)
#include "G4EmStandardPhysics_option3.hh" ///< EM physics with high-accuracy multiple scattering

// --- Hadronic Physics (Elastic & Inelastic) ---
#include "G4HadronElasticPhysicsHP.hh" ///< High-precision neutron elastic scattering
#include "G4HadronElasticPhysicsXS.hh" ///< Cross-section based hadron elastic scattering
#include "G4HadronInelasticQBBC.hh" ///< Alternative inelastic hadronic physics
#include "G4HadronPhysicsFTFP_BERT_HP.hh" ///< FTFP_BERT with HP neutron models
#include "G4HadronPhysicsINCLXX.hh"       ///< Liège intranuclear cascade
#include "G4HadronPhysicsQGSP_BIC_HP.hh" ///< Binary cascade + HP neutron physics

// --- Ion Physics ---
#include "G4IonElasticPhysics.hh" ///< Ion elastic scattering
#include "G4IonINCLXXPhysics.hh"  ///< Ion inelastic physics using INCLXX model
#include "G4IonPhysicsXS.hh" ///< Ion inelastic physics using cross-section data

// --- Stopping Physics ---
#include "G4StoppingPhysics.hh" ///< Stopping of charged particles (e.g., muons)

// --- Optical Physics ---
#include "G4OpticalParameters.hh"
#include "G4OpticalPhysics.hh"

// =============================
// OpticalSimulationPhysics Class
// =============================

/**
 * @class OpticalSimulationPhysics
 * @brief Modular physics list for PALLAS simulations.
 *
 * This class registers the set of physics processes used in the simulation.
 * The configuration prioritizes high-precision neutron physics and
 * detailed EM modeling for plasma and nuclear physics applications.
 *
 * Example usage:
 * @code
 * auto physicsList = new OpticalSimulationPhysics();
 * runManager->SetUserInitialization(physicsList);
 * @endcode
 */
class OpticalSimulationPhysics final : public G4VModularPhysicsList {
  public:
    /// Constructor: registers the selected physics modules
    OpticalSimulationPhysics();

    /// Destructor
    ~OpticalSimulationPhysics() override;
};

#endif // OpticalSimulationPhysics_h
