/**
 * @file OpticalSimulationPrimaryGeneratorAction.cc
 * @brief Implementation of the primary generator action for the Optical
 * simulation.
 *
 * This file implements the `OpticalSimulationPrimaryGeneratorAction` class,
 * which handles the generation of primary particles for Geant4 events in
 * optical simulation.
 *
 *  1. **Geant4 GeneralParticleSource (GPS)**: Standard Geant4 particle
 * generation.
 *
 * Features:
 *  - Thread-safe generation using atomic counters and per-thread UI handling.
 *  - Progress display with estimated remaining simulation time.
 *
 * Usage:
 *  - Instantiate the class and assign it to the Geant4 run manager via
 * `SetUserAction`.
 *
 * @note Multithreading is supported; thread 0 handles progress display.
 *
 * @authors Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 */

#include "OpticalSimulationPrimaryGeneratorAction.hh"

/// Global counter of generated particles (atomic to support multithreading).
std::atomic<size_t> currentParticleNumber{0};

/// Global pointer to the Geant4 UI manager.
G4UImanager *UI = G4UImanager::GetUIpointer();

/**
 * @brief Constructor for the primary generator action.
 *
 * Initializes the particle source, and
 * the associated UI messenger.
 *
 * @param N Total number of events to generate.
 * @param numThreads Number of threads used for multithreaded execution.
 * @param pMT Flag indicating whether multithreading is enabled.
 */
OpticalSimulationPrimaryGeneratorAction::
    OpticalSimulationPrimaryGeneratorAction(size_t N, size_t numThreads,
                                            G4bool pMT)
    : NEventsGenerated(N), numThreads(numThreads),
      G4VUserPrimaryGeneratorAction(), flag_MT(pMT) {
    particleSource = new G4GeneralParticleSource();
}

/**
 * @brief Destructor.
 *
 * Cleans up allocated resources such as the particle gun and particle source.
 */
OpticalSimulationPrimaryGeneratorAction::
    ~OpticalSimulationPrimaryGeneratorAction() {
    delete particleSource;
}

/**
 * @brief Display the progress of event generation.
 *
 * Prints a progress bar along with an estimate of the remaining simulation
 * time.
 *
 * @param progress Fraction of events completed (between 0 and 1).
 * @param startTime Start time of the simulation.
 */
void OpticalSimulationPrimaryGeneratorAction::ShowProgress(
    const double progress,
    std::chrono::high_resolution_clock::time_point startTime) {
    int barWidth = 70;
    std::ostringstream oss;

    oss << "\r["; // retour chariot pour écraser la ligne
    int pos = static_cast<int>(barWidth * progress);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos)
            oss << "=";
        else if (i == pos)
            oss << ">";
        else
            oss << " ";
    }
    oss << "] " << int(progress * 100.0) << " %";

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(
                           currentTime - startTime)
                           .count();

    double averageTimePerProgress =
        (progress > 0.0) ? (elapsedTime / progress) : 0.0;
    double estimatedRemainingTime = (1.0 - progress) * averageTimePerProgress;

    oss << " | ETA = " << std::fixed << std::setprecision(1)
        << estimatedRemainingTime / 60.0 << " min";

    if (progress > 0.01)
        std::cerr << oss.str() << std::flush;
}

/**
 * @brief Generate primary particles for a simulation event.
 *
 *  - Uses the Geant4 GeneralParticleSource (GPS) to generate the particle.
 *
 * @param anEvent Pointer to the Geant4 event where primary particles are
 * generated.
 */
void OpticalSimulationPrimaryGeneratorAction::GeneratePrimaries(
    G4Event *anEvent) {
    if (!isStartTimeInitialized) {
        startTime = std::chrono::high_resolution_clock::now();
        isStartTimeInitialized = true;
    }

    // ############################ CASE 1 : GENERATION FROM GPS
    // ############################
    particleSource->GeneratePrimaryVertex(anEvent);
    ++currentParticleNumber;

    if (threadID == 0) {
        ShowProgress(static_cast<double>(currentParticleNumber) /
                         double(NEventsGenerated),
                     startTime);
    }
}
