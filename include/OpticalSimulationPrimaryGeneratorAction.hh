#ifndef OpticalSimulationPrimaryGeneratorAction_h
#define OpticalSimulationPrimaryGeneratorAction_h 1

/**
 * @class OpticalSimulationPrimaryGeneratorAction
 * @brief Defines the primary generator action for the simulation.
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 *
 * This class controls the generation of primary particles in the Geant4
 * simulation.
 */

#include "G4GeneralParticleSource.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "OpticalSimulationRunAction.hh"

// Forward declarations
class G4ParticleGun;
class G4Event;

class OpticalSimulationPrimaryGeneratorAction
    : public G4VUserPrimaryGeneratorAction {
  public:
    /**
     * @brief Constructor.
     * @param N Number of events to generate.
     * @param numThreads Number of threads in multithreaded mode.
     * @param pMT Flag to enable multithreading.
     */
    OpticalSimulationPrimaryGeneratorAction(size_t N, size_t numThreads,
                                            bool pMT);

    /**
     * @brief Destructor.
     */
    ~OpticalSimulationPrimaryGeneratorAction();

    /**
     * @brief Generate primary particles for the event.
     * @param anEvent Pointer to the current G4Event.
     */
    void GeneratePrimaries(G4Event *anEvent) override;

  private:
    G4GeneralParticleSource *particleSource =
        nullptr; /**< General particle source */

    /**
     * @brief Display progress of event generation.
     * @param progress Current progress (0.0–1.0).
     * @param startTime Time when event generation started.
     */
    static void
    ShowProgress(double progress,
                 std::chrono::high_resolution_clock::time_point startTime);

    size_t NEventsGenerated = 0; /**< Number of events already generated */
    size_t currentEvent = 0;     /**< Current event index */
    size_t numThreads = 0;       /**< Number of threads */
    bool flag_MT = false;        /**< Flag indicating multithreading mode */
    int threadID = 0;            /**< Thread ID */
    int eventID = 0;             /**< Event ID */
    int nEvent = 0;              /**< Number of events processed */
    std::chrono::duration<double> elapsedTime; /**< Elapsed time since start */
    bool isStartTimeInitialized =
        false; /**< Flag indicating start time initialized */
    std::chrono::high_resolution_clock::time_point startTime; /**< Start time */
    double progressRemaining = 0.;      /**< Remaining progress fraction */
    double averageTimePerProgress = 0.; /**< Average time per progress step */
    double estimatedRemainingTime = 0.; /**< Estimated remaining time (s) */
};

#endif
