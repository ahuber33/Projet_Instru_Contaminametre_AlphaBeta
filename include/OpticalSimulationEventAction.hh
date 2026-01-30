#ifndef OpticalSimulationEventAction_h
#define OpticalSimulationEventAction_h 1

/**
 * @class OpticalSimulationEventAction
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 * @brief Header file for event-level actions in OpticalSimulation, including
 * per-event statistics structures.
 */

#include "G4String.hh"
#include "G4Types.hh"
#include "G4UserEventAction.hh"
#include <TBranch.h>
#include <TTree.h>
#include <vector>

class G4Event;

/**
 * @brief Structure to store per-event input particle data
 *
 * Holds position, direction, and energy information for the particle at the
 * start of the event.
 */
struct RunTallyInput {
    float x = 0.0;
    float xp = 0.0;
    float y = 0.0;
    float yp = 0.0;
    float z = 0.0;
    float zp = 0.0;
    float energy = 0.0;
};

// This struct carries statistics OPTICAL part
struct RunTallyOptical {
    float IncidentE;
    float DepositTotal;
    float DepositZnS;
    float DepositSc;
    G4int GeneratedTotal;
    G4int GeneratedZnS;
    G4int ScintillationZnS;
    G4int CerenkovZnS;
    G4int GeneratedSc;
    G4int ScintillationSc;
    G4int CerenkovSc;
    G4int BulkAbsTotal;
    G4int BulkAbsZnS;
    G4int BulkAbsSc;
    G4int Absorbed;
    G4int Escaped;
    G4int Failed;
    G4int Killed;
    G4int Detected;
    std::vector<float> ExitLightPositionX;
    std::vector<float> ExitLightPositionY;
    std::vector<float> ExitLightPositionZ;
    std::vector<float> DetectorPositionX;
    std::vector<float> DetectorPositionY;
    std::vector<float> DetectorPositionZ;
    std::vector<float> BirthWavelength;
    std::vector<float> BirthWavelengthDetected;
    std::vector<float> Time;
    std::vector<int> Rayleigh;
    std::vector<int> Total_Reflections;
    std::vector<int> Wrap_Reflections;
    std::vector<float> TotalLength;
    std::vector<float> Angle_creation;
    std::vector<float> Angle_detection;
    std::vector<int> FinalState;

    inline G4int operator==(const RunTallyOptical &right) const {
        return (this == &right);
    }
};

/**
 * @brief Structure for YAG detector statistics
 *
 * Tracks exit coordinates, particle IDs, energies, deposited energy, and flags.
 */
struct RunTallySc {
    std::vector<float> x_entrance;
    std::vector<float> y_entrance;
    std::vector<float> z_entrance;
    std::vector<int> parentID;
    std::vector<int> particleID;
    std::vector<float> energy;
    float deposited_energy = 0.0;
    float deposited_energy_event = 0.0;
    std::vector<float> total_deposited_energy;
    G4bool flag = false;

    // Methods to add data
    void AddXEntrance(float d) { x_entrance.push_back(d); }
    void AddYEntrance(float d) { y_entrance.push_back(d); }
    void AddZEntrance(float d) { z_entrance.push_back(d); }
    void AddParentID(int d) { parentID.push_back(d); }
    void AddParticleID(int d) { particleID.push_back(d); }
    void AddEnergy(float d) { energy.push_back(d); }
    void AddDepositedEnergyEvent(float d) { deposited_energy_event += d; }
    void AddDepositedEnergy(float d) { deposited_energy += d; }
    void AddTotalDepositedEnergy(float d) {
        total_deposited_energy.push_back(d);
    }

    // Size accessors
    size_t XEntranceSize() const { return x_entrance.size(); }
    size_t YEntranceSize() const { return y_entrance.size(); }
    size_t ZEntranceSize() const { return z_entrance.size(); }
    size_t ParentIDSize() const { return parentID.size(); }
    size_t ParticleIDSize() const { return particleID.size(); }
    size_t EnergySize() const { return energy.size(); }
    size_t TotalDepositedEnergySize() const {
        return total_deposited_energy.size();
    }

    // Index accessors
    float GetXEntrance(size_t i) const { return x_entrance.at(i); }
    float GetYEntrance(size_t i) const { return y_entrance.at(i); }
    float GetZEntrance(size_t i) const { return z_entrance.at(i); }
    int GetParentID(size_t i) const { return parentID.at(i); }
    int GetParticleID(size_t i) const { return particleID.at(i); }
    float GetEnergy(size_t i) const { return energy.at(i); }
    float GetTotalDepositedEnergy(size_t i) const {
        return total_deposited_energy.at(i);
    }
    float GetTotalDepositedEnergyEvent() const {
        return deposited_energy_event;
    }

    // Flags
    void ActivateFlag() { flag = true; }
    void ResetFlag() { flag = false; }
    G4bool ReturnFlag() const { return flag; }

    void ResetDepositedEnergy() { deposited_energy = 0; }
    float GetDepositedEnergy() const { return deposited_energy; }
};

/**
 * @brief Event action class for OpticalSimulation
 *
 * Implements G4UserEventAction interface to handle per-event statistics,
 * including input particle, collimator, quadrupole, and detector data.
 */
class OpticalSimulationEventAction : public G4UserEventAction {
  public:
    /** Constructor */
    OpticalSimulationEventAction(const char *);

    /** Destructor */
    ~OpticalSimulationEventAction();

    /** Called at the beginning of each event */
    void BeginOfEventAction(const G4Event *);

    /** Called at the end of each event */
    void EndOfEventAction(const G4Event *);

    /** Setters for input particle data */
    void SetXStart(G4float d) { StatsInput.x = d; }
    void SetXpStart(G4float d) { StatsInput.xp = d; }
    void SetYStart(G4float d) { StatsInput.y = d; }
    void SetYpStart(G4float d) { StatsInput.yp = d; }
    void SetZStart(G4float d) { StatsInput.z = d; }
    void SetZpStart(G4float d) { StatsInput.zp = d; }
    void SetEnergyStart(G4float d) { StatsInput.energy = d; }

    // ░█████╗░██████╗░████████╗██╗░█████╗░░█████╗░██╗░░░░░
    // ██╔══██╗██╔══██╗╚══██╔══╝██║██╔══██╗██╔══██╗██║░░░░░
    // ██║░░██║██████╔╝░░░██║░░░██║██║░░╚═╝███████║██║░░░░░
    // ██║░░██║██╔═══╝░░░░██║░░░██║██║░░██╗██╔══██║██║░░░░░
    // ╚█████╔╝██║░░░░░░░░██║░░░██║╚█████╔╝██║░░██║███████╗
    // ░╚════╝░╚═╝░░░░░░░░╚═╝░░░╚═╝░╚════╝░╚═╝░░╚═╝╚══════╝

    // Functions for Optical Tree
    void CountCerenkovZnS() { StatsOptical.CerenkovZnS++; }
    void CountCerenkovSc() { StatsOptical.CerenkovSc++; }
    void CountScintillationZnS() { StatsOptical.ScintillationZnS++; }
    void CountScintillationSc() { StatsOptical.ScintillationSc++; }
    float GetScintillationSc() { return StatsOptical.ScintillationSc; }
    float GetCerenkovSc() { return StatsOptical.CerenkovSc; }
    void CountKilled() { StatsOptical.Killed++; }
    int GetKilled() { return StatsOptical.Killed; }
    void CountDetected() { StatsOptical.Detected++; }
    int GetDetected() { return StatsOptical.Detected; }
    // void CountWLS(){StatsOptical.WLS++;}
    void CountAbsorbed() { StatsOptical.Absorbed++; }
    int GetAbsorbed() { return StatsOptical.Absorbed; }
    void CountBulkAbsSc() { StatsOptical.BulkAbsSc++; }
    int GetBulkAbsSc() { return StatsOptical.BulkAbsSc; }
    void CountBulkAbsZnS() { StatsOptical.BulkAbsZnS++; }
    int GetBulkAbsZnS() { return StatsOptical.BulkAbsZnS; }
    void CountEscaped() { StatsOptical.Escaped++; }
    int GetEscaped() { return StatsOptical.Escaped; }
    void CountFailed() { StatsOptical.Failed++; }
    int GetFailed() { return StatsOptical.Failed; }
    void FillPhotonExitLightPositionX(float e) {
        StatsOptical.ExitLightPositionX.push_back(e);
    }
    void FillPhotonExitLightPositionY(float e) {
        StatsOptical.ExitLightPositionY.push_back(e);
    }
    void FillPhotonExitLightPositionZ(float e) {
        StatsOptical.ExitLightPositionZ.push_back(e);
    }
    void FillPhotonDetectorPositionX(float e) {
        StatsOptical.DetectorPositionX.push_back(e);
    }
    void FillPhotonDetectorPositionY(float e) {
        StatsOptical.DetectorPositionY.push_back(e);
    }
    void FillPhotonDetectorPositionZ(float e) {
        StatsOptical.DetectorPositionZ.push_back(e);
    }
    void FillPhotonFinalState(G4int e) { StatsOptical.FinalState.push_back(e); }
    void FillBirthWavelength(float e) {
        StatsOptical.BirthWavelength.push_back(e);
    }
    void FillBirthWavelengthDetected(float e) {
        StatsOptical.BirthWavelengthDetected.push_back(e);
    }
    void FillPhotonTime(float e) { StatsOptical.Time.push_back(e); }
    void FillRayleigh(int e) { StatsOptical.Rayleigh.push_back(e); }
    void FillTotalReflections(int e) {
        StatsOptical.Total_Reflections.push_back(e);
    }
    void FillWrapReflecions(int e) {
        StatsOptical.Wrap_Reflections.push_back(e);
    }
    void FillPhotonTotalLength(float e) {
        StatsOptical.TotalLength.push_back(e);
    }
    void FillFiberAngleCreation(float e) {
        StatsOptical.Angle_creation.push_back(e);
    }

    void FillFiberAngleDetection(float e) {
        StatsOptical.Angle_detection.push_back(e);
    }
    void SetAirIndex(float a) { Air_Index = a; }
    float GetAirIndex() { return Air_Index; }

    /** Accessors for generic detector statistics */
    RunTallySc &GetZnS() { return StatsZnS; }
    RunTallySc &GetScintillator() { return StatsScintillator; }

  private:
    TTree *EventTree;         ///< ROOT tree for per-event data
    TBranch *EventBranch;     ///< ROOT branch for event tree
    RunTallyInput StatsInput; ///< Input particle statistics
    RunTallySc StatsZnS;      ///< Beam Stop ZnS detector statistics
    RunTallySc
        StatsScintillator; ///< Beam Stop SPEC Scintillator detector statistics
    G4String suffixe;      ///< Suffix for output naming
    RunTallyOptical StatsOptical;
    float Photon_creation_angle = 0.0;
    float Photon_creation_lambda = 0.0;
    float Air_Index = 0.0;
    float Absfrac = 0.0;
    float BulkfracZnS = 0.0;
    float BulkfracSc = 0.0;
    float Escfrac = 0.0;
    float Failfrac = 0.0;
    float efficiency = 0.0;
    G4int GeneratedSc = 0;
    G4int GeneratedZnS = 0;
    G4int Generated = 0;
    G4int Scintillation = 0;
    G4int Cerenkov = 0;
    float Deposit = 0.0;
    G4bool VerbosityResults = false;
};

#endif
