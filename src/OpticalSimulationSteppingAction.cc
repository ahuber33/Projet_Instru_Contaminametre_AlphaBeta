/**
 * @file OpticalSimulationSteppingAction.cc
 * @brief Implements step-level tracking, data extraction, and event updates for
 * the Optical simulation.
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 *
 * This file contains the method definitions for the
 * `OpticalSimulationSteppingAction` class declared in
 * `OpticalSimulationSteppingAction.hh`. It manages:
 *  - Extraction of particle positions, momentum, energy, and track metadata at
 * each Geant4 step
 *
 * [TO DO]
 *
 * @note This class relies on Geant4's step action mechanism to record
 * fine-grained tracking information suitable for later beamline analysis.
 */

#include "OpticalSimulationSteppingAction.hh"

/**
 * @brief Constructor.
 *
 * Initializes the Geant4 generic messenger and declares user commands
 * for controlling tracking status (global and for collimators).
 */
OpticalSimulationSteppingAction::OpticalSimulationSteppingAction() {
    sMessenger = new G4GenericMessenger(this, "/OpticalSimulation/step/",
                                        "Control commands for my application");

    sMessenger->DeclareProperty("setVerbose", VerbosityLevel)
        .SetGuidance("Set the verbosity level.")
        .SetParameterName("VerbosityLevel", false)
        .SetDefaultValue("0");

    sMessenger->DeclareProperty("setPhotonTrackStatus", PhotonTrackStatus)
        .SetGuidance("Set the photon track status.")
        .SetParameterName("PhotonTrackStatus", false)
        .SetDefaultValue("true");
}

/**
 * @brief Destructor.
 *
 * Cleans up the allocated messenger.
 */
OpticalSimulationSteppingAction::~OpticalSimulationSteppingAction() {
    delete sMessenger;
}

/**
 * @brief Store the initial beam parameters at the beginning of the event.
 *
 * @param evtac Pointer to the event action where the values are stored.
 */
void OpticalSimulationSteppingAction::SetInputInformations(
    OpticalSimulationEventAction *evtac) const {
    evtac->SetXStart(preStep.x);
    evtac->SetXpStart(preStep.px);
    evtac->SetYStart(preStep.y);
    evtac->SetYpStart(preStep.py);
    evtac->SetZStart(preStep.z);
    evtac->SetZpStart(preStep.pz);
    evtac->SetEnergyStart(energy);
}

void OpticalSimulationSteppingAction::CheckBoundaryStatus(
    const G4Step *aStep, OpticalSimulationEventAction *evtac) {
    G4OpBoundaryProcessStatus boundaryStatus = Undefined;
    G4OpBoundaryProcess *boundary = NULL;

    // find the boundary process only once
    if (!boundary) {
        G4ProcessManager *pm =
            aStep->GetTrack()->GetDefinition()->GetProcessManager();
        G4int nprocesses = pm->GetProcessListLength();
        G4ProcessVector *pv = pm->GetProcessList();
        G4int i;
        for (i = 0; i < nprocesses; i++) {
            if ((*pv)[i]->GetProcessName() == "OpBoundary") {
                boundary = (G4OpBoundaryProcess *)(*pv)[i];
                break;
            }
        }
    }

    boundaryStatus = boundary->GetStatus();

    if (endproc == "OpAbsorption") {
        if (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "ZnS") {
            evtac->CountBulkAbsZnS();
        } else {
            evtac->CountBulkAbsSc();
        }
        if (VerbosityLevel > 1)
            G4cout << "Photon BulkAbsorbed" << G4endl;
        // evtac->FillFiberAngleCreation(evtac->GetPhotonCreationAngle());
    }

    if (endproc == "OpRayleigh") {
        //((OpticalSimulationTrackInformation*)(aStep->GetTrack()->GetUserInformation()))->CountRayleighScattering();
        // G4cout << "Rayleigh scattering" << G4endl;
        // G4cout << "Number of scattering = " <<
        // ((ENLOpticalSimTrackInformation*)
        // (aStep->GetTrack()->GetUserInformation()))->GetRayleigh() << G4endl;
    }

    else if (particleName == "opticalphoton" && endproc != "Transportation" &&
             endproc != "OpAbsorption")
        G4cout << endproc << G4endl;

    if (aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
        if (VerbosityLevel > 1)
            G4cout << "Boundary Status = " << boundaryStatus << G4endl;

        switch (boundaryStatus) {
        case Detection:
            evtac->CountDetected();
            evtac->FillPhotonDetectorPositionX(postStep.x);
            evtac->FillPhotonDetectorPositionY(postStep.y);
            evtac->FillPhotonDetectorPositionZ(postStep.z);
            evtac->FillBirthWavelengthDetected(
                1240 / (theTrack->GetTotalEnergy() / eV));
            evtac->FillPhotonTime(aStep->GetPostStepPoint()->GetGlobalTime() /
                                  ns);
            evtac->FillPhotonTotalLength(aStep->GetTrack()->GetTrackLength());

            if (VerbosityLevel > 1) {
                G4cout << "Photon detecté" << G4endl;
                G4cout << "N detecté = " << evtac->GetDetected() << G4endl;
            }

            break;

        case Absorption: // used to get the number TRANSMITTED!!
            if (theTrack->GetNextVolume()->GetName() == "Photocathode") {
                evtac->CountFailed();

                if (VerbosityLevel > 1) {
                    G4cout << "Photon failed" << G4endl;
                    G4cout << "N failed = " << evtac->GetFailed() << G4endl;
                }
            } else { // if not bulk, transmitted, or detected...it must be
                     // surface!
                evtac->CountAbsorbed();
                // evtac->FillFiberAngleCreation(evtac->GetPhotonCreationAngle());

                if (VerbosityLevel > 1) {
                    G4cout << "Photon surface absorbed" << G4endl;
                    G4cout << "N absorbed = " << evtac->GetAbsorbed() << G4endl;
                }
            }
            break;

        case Undefined:
            if (VerbosityLevel > 1)
                G4cout << "Undefined Boundary Process!" << G4endl;
            break;

        case NoRINDEX:
            evtac->CountEscaped();
            // evtac->FillFiberAngleCreation(evtac->GetPhotonCreationAngle());

            if (VerbosityLevel > 1) {
                G4cout << "count escaped" << G4endl;
                G4cout << "N escaped = " << evtac->GetEscaped() << G4endl;
            }
            break;

        // if we have any kind of reflections, count them
        case LambertianReflection:
            if (VerbosityLevel > 1)
                G4cout << "Reflection L" << G4endl;
            break;

        case FresnelRefraction:
            if (VerbosityLevel > 1)
                G4cout << "Fresnel Refraction" << G4endl;
            break;

        case FresnelReflection:
            if (VerbosityLevel > 1)
                G4cout << "Fresnel Reflection" << G4endl;
            break;

        case LobeReflection:
            if (VerbosityLevel > 1)
                G4cout << "Reflection Lobe" << G4endl;
            break;

        case SpikeReflection:
            //((OpticalSimulationTrackInformation*)(aStep->GetTrack()->GetUserInformation()))->CountReflections();
            if (VerbosityLevel > 1)
                G4cout << "Reflection" << G4endl;
            break;

        case TotalInternalReflection:
            //((OpticalSimulationTrackInformation*)(aStep->GetTrack()->GetUserInformation()))->CountTotalInternalReflections();
            if (VerbosityLevel > 1)
                G4cout << "Reflection totale" << G4endl;
            break;

        default:
            break;
        }
    }
}

void OpticalSimulationSteppingAction::CountScintillation(
    const G4Step *aStep, OpticalSimulationEventAction *evtac) {
    if (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "ZnS") {
        evtac->CountScintillationZnS();
        if (VerbosityLevel > 1)
            G4cout << " Photon Scintillation from ZnS!!!" << G4endl;
    }

    if (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() ==
        "Scintillator")
        evtac->CountScintillationSc();
    if (VerbosityLevel > 1)
        G4cout << " Photon Scintillation from Sc!!!" << G4endl;
}

void OpticalSimulationSteppingAction::CountCerenkov(
    const G4Step *aStep, OpticalSimulationEventAction *evtac) {
    if (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "ZnS") {
        evtac->CountCerenkovZnS();
    }
    if (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() ==
        "Scintillator") {
        evtac->CountCerenkovSc();
    }
    if (VerbosityLevel > 1)
        G4cout << " Photon Cerenkov !!!" << G4endl;
}

void OpticalSimulationSteppingAction::SetPhotonBirthInformation(
    const G4Step *aStep, OpticalSimulationEventAction *evtac) {
    evtac->FillFiberAngleCreation(angle / deg);
    evtac->FillBirthWavelength(1240 / (theTrack->GetTotalEnergy() / eV));
    if (VerbosityLevel > 0) {
        G4cout << "Birth Wavelength = "
               << 1240 / (theTrack->GetTotalEnergy() / eV) << G4endl;
        G4cout << "Angle creation = " << angle / deg << G4endl;
    }
}

/**
 * @brief Update YAG (Yttrium Aluminum Garnet screen) tally with particle data.
 *
 * @param tally YAG tally to update.
 * @param x Exit X position [mm].
 * @param y Exit Y position [mm].
 * @param z Exit Z position [mm].
 * @param energy Particle kinetic energy [MeV].
 * @param energyDeposited Deposited energy [keV].
 * @param parentID ID of the parent track.
 * @param particleID PDG encoding of the particle.
 * @param volumeNamePostStep Name of the post-step volume.
 * @param trackingStatus Whether particle tracking is active.
 * @param track Pointer to the current Geant4 track.
 */
void UpdateSc(RunTallySc &tally, G4float x, G4float y, G4float z,
              G4float energy, G4float energyDeposited, G4float energy_post,
              G4float parentID, G4int particleID,
              const G4String &volumeNamePostStep, G4bool trackingStatus,
              G4Track *track) {
    // If this is the first step for this particle in this tally
    if (!tally.flag) {
        tally.AddXEntrance(x);
        tally.AddYEntrance(y);
        tally.AddZEntrance(z);
        tally.AddParentID(parentID);
        tally.AddParticleID(particleID);
        tally.AddEnergy(energy);
        tally.ActivateFlag();
    }

    // Add energy deposited for this step
    tally.AddDepositedEnergy(energyDeposited);
    tally.AddDepositedEnergyEvent(energyDeposited);

    // If particle reached holder volume or lost all energy
    if (volumeNamePostStep == "Holder" || energy_post == 0) {
        tally.AddTotalDepositedEnergy(tally.GetDepositedEnergy());
        tally.ResetDepositedEnergy();
        tally.ResetFlag();
        G4cout << "RESET SC" << G4endl;
    }
}

/**
 * @brief Main Geant4 stepping action executed at each simulation step.
 *
 * Collects track and step information (positions, momenta, energies, volumes).
 * Manages input beam initialization, quadrupole crossing, collimator detection,
 * YAG screens, and particle termination.
 *
 * @param aStep Pointer to the current Geant4 step.
 */
void OpticalSimulationSteppingAction::UserSteppingAction(const G4Step *aStep) {
    // --- Preparation of variables ---
    auto evtac = static_cast<OpticalSimulationEventAction *>(
        G4EventManager::GetEventManager()->GetUserEventAction());
    theTrack = aStep->GetTrack();
    auto pre = aStep->GetPreStepPoint();
    auto post = aStep->GetPostStepPoint();

    // General info
    particleName = theTrack->GetDefinition()->GetParticleName();
    particleID = theTrack->GetDefinition()->GetPDGEncoding();
    endproc = post->GetProcessDefinedStep()->GetProcessName();
    parentID = theTrack->GetParentID();
    trackID = theTrack->GetTrackID();
    stepNo = theTrack->GetCurrentStepNumber();
    energy = pre->GetKineticEnergy() / MeV;
    energy_post = post->GetKineticEnergy() / MeV;
    energyDeposited = aStep->GetTotalEnergyDeposit() / CLHEP::keV;
    angle = acos((postStep.x - preStep.x) / aStep->GetStepLength());
    time = aStep->GetPostStepPoint()->GetGlobalTime() / ns;

    // Positions
    auto prePos = pre->GetPosition() / CLHEP::mm;
    auto postPos = post->GetPosition() / CLHEP::mm;
    preStep.x = prePos.x();
    preStep.y = prePos.y();
    preStep.z = prePos.z();
    postStep.x = postPos.x();
    postStep.y = postPos.y();
    postStep.z = postPos.z();

    // Directions
    auto preMom = pre->GetMomentumDirection();
    auto postMom = post->GetMomentumDirection();
    preStep.px = preMom.x();
    preStep.py = preMom.y();
    preStep.pz = preMom.z();
    postStep.px = postMom.x();
    postStep.py = postMom.y();
    postStep.pz = postMom.z();

    // Volumes
    volumeNamePreStep = pre->GetPhysicalVolume()->GetName();
    volumeNamePostStep = post->GetPhysicalVolume()->GetName();

    // --- Begin main logic ---

    // Initial beam info (step 1, primary particle only)
    if (parentID == 0 && stepNo == 1)
        SetInputInformations(evtac);

    // YAG screens
    static const std::map<std::string,
                          RunTallySc &(OpticalSimulationEventAction::*)()>
        ScMap = {
            {"ZnS", &OpticalSimulationEventAction::GetZnS},
            {"Scintillator", &OpticalSimulationEventAction::GetScintillator},
        };

    auto it = ScMap.find(volumeNamePreStep);
    if (it != ScMap.end() && particleName != "opticalphoton") {
        RunTallySc &sc = (evtac->*(it->second))();
        UpdateSc(sc, preStep.x, preStep.y, preStep.z, energy, energyDeposited,
                 energy_post, parentID, particleID, volumeNamePostStep,
                 TrackingStatus, theTrack);
    }

    // ░█████╗░██████╗░████████╗██╗░█████╗░░█████╗░██╗░░░░░  ██████╗░░█████╗░██████╗░████████╗
    // ██╔══██╗██╔══██╗╚══██╔══╝██║██╔══██╗██╔══██╗██║░░░░░  ██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝
    // ██║░░██║██████╔╝░░░██║░░░██║██║░░╚═╝███████║██║░░░░░  ██████╔╝███████║██████╔╝░░░██║░░░
    // ██║░░██║██╔═══╝░░░░██║░░░██║██║░░██╗██╔══██║██║░░░░░  ██╔═══╝░██╔══██║██╔══██╗░░░██║░░░
    // ╚█████╔╝██║░░░░░░░░██║░░░██║╚█████╔╝██║░░██║███████╗  ██║░░░░░██║░░██║██║░░██║░░░██║░░░
    // ░╚════╝░╚═╝░░░░░░░░╚═╝░░░╚═╝░╚════╝░╚═╝░░╚═╝╚══════╝  ╚═╝░░░░░╚═╝░░╚═╝╚═╝░░╚═╝░░░╚═╝░░░

    if (particleName == "opticalphoton") {
        if (PhotonTrackStatus == false) {
            evtac->CountKilled();
            theTrack->SetTrackStatus(fStopAndKill);
        }

        else {
            CheckBoundaryStatus(aStep, evtac);
        }

        if (stepNo == 1) {
            SetPhotonBirthInformation(aStep, evtac);
            if (aStep->GetTrack()->GetCreatorProcess()->GetProcessName() ==
                "Scintillation")
                CountScintillation(aStep, evtac);
            if (aStep->GetTrack()->GetCreatorProcess()->GetProcessName() ==
                "Cerenkov")
                CountCerenkov(aStep, evtac);
        }
    }

    // TPSimTrackInformation *info = static_cast<TPSimTrackInformation
    // *>(aStep->GetTrack()->GetUserInformation());
    //  if (!info && partname == "opticalphoton")
    //  {
    //    G4Exception("TPSimTrackInformation", "NoTrackInfo",
    //                FatalException, "No User Information available for this
    //                track!");
    //  }

    if (VerbosityLevel > 0) {
        G4cout << "x = " << preStep.x << G4endl;
        G4cout << "y = " << preStep.y << G4endl;
        G4cout << "z = " << preStep.z << G4endl;
        G4cout << "px = " << preStep.px << G4endl;
        G4cout << "py = " << preStep.py << G4endl;
        G4cout << "pz = " << preStep.pz << G4endl;
        G4cout << "angle = " << angle / deg << G4endl;
        G4cout << "Time = " << time << " ns" << G4endl;
    }

    if (VerbosityLevel > 1) {
        int abs = evtac->GetBulkAbsSc();
        int esc = evtac->GetEscaped();
        int failed = evtac->GetFailed();
        int det = evtac->GetDetected();
        int killed = evtac->GetKilled();
        int tot = abs + esc + failed + det + killed;

        G4cout << "\nN killed = " << killed << G4endl;
        G4cout << "N abs = " << abs << G4endl;
        G4cout << "N esc = " << esc << G4endl;
        G4cout << "N failed = " << failed << G4endl;
        G4cout << "N det = " << det << G4endl;
        G4cout << "N TOT = " << tot << G4endl;
        G4cout << "N Scintillation actuel = " << evtac->GetScintillationSc()
               << G4endl;
        G4cout << "N Cerenkov actuel = " << evtac->GetCerenkovSc() << G4endl;
        G4cout << "N tot Sc + Cerenkov = "
               << evtac->GetScintillationSc() + evtac->GetCerenkovSc()
               << G4endl;
    }

    if (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "World") {
        theTrack->SetTrackStatus(fStopAndKill);
    }
}
