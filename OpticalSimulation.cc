#include "G4MTRunManager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "Geometry.hh"
#include "OpticalSimulationActionInitialization.hh"
#include "OpticalSimulationPhysics.hh"
#include <thread>
#include "G4UImanager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"

int main(int argc, char **argv) {
    if (argc < 2) {
        G4Exception("Main", "main0004", FatalException,
                    "Insufficient input arguments. Usage: ./OpticalSimulation [ROOT file name] [events] [macro] [MT ON/OFF] [threads]");
        return 1;
    }

    char *outputFile = argv[1];
    size_t TotalNParticles = 0;
    bool flag_MT = false;
    size_t Ncores = std::thread::hardware_concurrency();
    G4RunManager *runManager;

    // Determine mode
    if (argc == 2) {
        runManager = new G4RunManager;
    } else if (argc >= 5) {
        TotalNParticles = std::stoul(argv[2]);
        G4String pMT = argv[4];
        if (pMT == "ON") {
            flag_MT = true;
            runManager = new G4MTRunManager;
            if (argc == 6) Ncores = std::stoul(argv[5]);
            runManager->SetNumberOfThreads(Ncores);
        } else if (pMT == "OFF") {
            flag_MT = false;
            runManager = new G4RunManager;
        } else {
            G4Exception("Main", "main0002", FatalException,
                        "MT parameter (5th argument) must be ON or OFF.");
        }
    } else {
        G4Exception("Main", "main0003", FatalException,
                    "Incorrect number of input parameters.");
    }

    // Geometry and physics
    Geometry *Geom = new Geometry();
    OpticalSimulationGeometryConstruction *GeomCons = new OpticalSimulationGeometryConstruction;
    runManager->SetUserInitialization(GeomCons);
    runManager->SetUserInitialization(new OpticalSimulationPhysics);
    runManager->SetUserInitialization(new OpticalSimulationActionInitialization(
        outputFile, TotalNParticles, Ncores, flag_MT, GeomCons));

    // --- Initialize visualization manager silently (no real window) ---
    G4VisManager *visManager = new G4VisExecutive("Quiet");
    visManager->Initialize();

    // Initialize kernel
    runManager->Initialize();

    G4UImanager *UI = G4UImanager::GetUIpointer();

    // Visualization mode
    if (argc == 2) {
        G4UIExecutive *ui = new G4UIExecutive(argc, argv);
        UI->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
        delete ui;
    }
    // Batch mode
    else if (argc >= 5) {
        G4String command = "/control/execute ";
        G4String macro = argv[3];
        UI->ApplyCommand(command + macro);

        std::string runCommand = "/run/beamOn " + std::string(argv[2]);
        UI->ApplyCommand(runCommand);

        // Merge ROOT files if MT
        if (flag_MT) {
            std::string mergeCommand = "/control/shell hadd -k -f " +
                                       std::string(outputFile) + ".root";
            for (size_t i = 1; i <= Ncores; ++i)
                mergeCommand += " " + std::string(outputFile) + "_" + std::to_string(i) + ".root";
            UI->ApplyCommand(mergeCommand);

            for (size_t i = 0; i <= Ncores; ++i) {
                std::string removeCommand = "/control/shell rm -f " +
                                            std::string(outputFile) + "_" + std::to_string(i) + ".root";
                UI->ApplyCommand(removeCommand);
            }
        }
    }

    std::string movefile = "/control/shell mv " + std::string(outputFile) + ".root ../Resultats";
    UI->ApplyCommand(movefile);
    G4cout << "Output saved in Resultats folder to file " << outputFile << ".root" << G4endl;

    delete visManager;
    delete runManager;

    return 0;
}
