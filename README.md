\mainpage PlasmaMLPALLAS
# OpticalSimulation_BASE – Primary brick of optical simulation in order to be adapted to diverse configurations

**Project Authors:** Arnaud HUBER
**Date:** 2025–2026  

---

## Table of Contents
[TO MODIFY]
- Introduction
- Features
- Dependencies
- Installation
- Building the Simulation
- Running the Simulation
- PlasmaMLPALLAS Macro Configuration
- Primary Generator
- ONNX Model Inference for Plasma Beam Generation
- Geometry Utilities
- PALLAS Geometry Construction
- PALLAS MagneticField
- Physics List
- Quadrupole Utilities
- Step-Level Tracking and Data Extraction
- ROOT Output
- Architecture Diagram
- Automated Testing
- Documentation
- License

---

## Introduction

[TO DO]]

🎯 Audience: Physicists, accelerator scientists, ML/physics integration developers.

🧩 Applications: Plasma accelerators, beamline design, detector response studies.

⚡ Strength: Combines ML inference + Geant4 physics + ROOT data analysis in a single workflow.

---

## Features

[TO DO]

---

## Dependencies

- Geant4 >= 11.1 with GDML option **ACTIVATED** [Compilation OK on version 11.3.2]
```
cmake -DGEANT4_USE_GDML=ON
```
- CLHEP (units & constants)
- C++17 or later
- Standard libraries: `<atomic>`, `<chrono>`, `<vector>`, `<string>`, `<cmath>`, `<memory>`

---

## Installation

```bash
git clone https://github.com/ahuber33/OpticalSimulation_BASE
```

---

## Building the Simulation

```bash
mkdir build && cd build
cmake -DGeant4_DIR=$G4COMP ../
make -j$(nproc)
```

The executable `OpticalSimulation` will be added to your `bin` folder.

---

## Running the Simulation

- **With visualization:**

```bash
./OpticalSimulation [name_of_ROOT_file]
```

It will generate particles according to the macro (e.g., `vis.mac`) and produce a ROOT file in the `Resultats` folder.

- **Without visualization (statistics only):**

```bash
./PlasmaMLPALLAS [name_of_ROOT_file] [number_of_events] [macro_file] [MT ON/OFF] [number_of_threads]
```

**Notes:**
- If MT is ON, temporary ROOT files for each thread are merged at the end.
- If MT is OFF, no need to specify the number of threads.
- You can use any macro file (e.g., `vrml.mac`).

---

## OpticalSimulation_BASE Macro Configuration [TO ADAPT]

This macro configures geometry, magnetic fields, tracking, and particle sources for PALLAS PlasmaMLP simulations. It allows full customization of quadrupole setup, dipole fields, and particle generation methods.

**Geometry Configuration:**

- Quadrupole lengths: Q1-Q4 individually configurable.
- Distances: between source, quadrupoles and collimators.
- Display options: toggle full geometry, quadrupoles and collimators for visualization.
- Changes are applied with `/run/reinitializeGeometry`.

**Magnetic Fields Configuration:**

- Quadrupole gradients: individually set for Q1-Q4.
- B-field map mode: can be enabled/disabled for dipole spectrometer
- Dipole field: constant value if B-field map mode is disabled.

**Tracking options Configuration:**

- Particle tracking can be enabled/disabled globally or for collimators interaction.
- Verbosity controls for both tracking and run output.

**Particle Sources Configuration:**

- ONNX-based Particle Gun:
  - Particle type, laser focus, normalized vector potential, dopant fraction and chamber pressure configurable.
- GEANT4 GPS (General Particle Source):
  - Number, type, position, direction and energy of particles.

This macro provides a ready-to-use setup for simulations with either the particle gun or GPS, including full control over magnetic fields and geometry.

**User Interface Commands:**
[TO DO]

**Controls:**
[TO DO & ADAPT]
- Display:
  - enable/disable geometry: `/display/setStatusDisplayGeometry`
  - enable/disable quadrupoles: `/display/setStatusDisplayQuadrupoles`
  - enable/disable collimators: `/display/setStatusDisplayCollimators`
  - enable/disable GDML collimators: `/display/setStatusDisplayGDMLCollimators`
- GPS (alternative to particle gun):
  - particle number: `/gps/number`
  - particle type: `/gps/particle`
  - particle position: `/gps/pos`
  - particle direction: `/gps/direction`
  - particle energy: `/gps/energy`

---

## Primary Generator

**Class:** `PlasmaMLPALLASPrimaryGeneratorAction`
[TO DO & ADAPT]

The PlasmaMLPALLASPrimaryGeneratorAction class handles the generation of primary particles for the PALLAS simulation. It supports two generation modes:

**Modes:**
- GPS particle source:
  - Standard particle generation using the Geant4 GPS

**Features:**

- Thread-safe generation using atomic counters.
- Per-thread UI handling; thread 0 displays a progress bar with estimated remaining time.
- Automatic particle definition lookup in the Geant4 particle table.
- Supports multithreading and safe progress display.

**Usage:**

- Instantiate the class and assign it to Geant4 run manager using `SetUserAction`
- Configure ONNX or GPS parameters via the messenger before starting the run.

---

**Units:**

- All physical quantities are expressed using CLHEP units.

---

## Geometry Utilities

**File:** `Geometry.cc`

The Geometry class provides tools to define and load geometrical volumes for the PALLAS simulation.
It centralizes the construction of detector and beamline components, either from GDML descriptions or via simplified Geant4 primitives.

**Features:**

- GDML Import: Load volumes from GDML files and assign materials dynamically.
- Quadrupole Construction: Generate simplified quadrupole magnets as vacuum-filled box volumes.
- Diagnostics Chambers: Create cylindrical chambers by subtracting tubular volumes, useful for fast detector prototyping.

**Example:**

```cpp
// Load a GDML-defined volume
auto* vol = geometry.GetGDMLVolume("beamline.gdml", "Target", material);

// Create a simple quadrupole
auto* quad = geometry.GetQuadrupoleVolume("Q1", 50, 50, 200);

// Build a diagnostic chamber
auto* chamber = geometry.GetFakeDiagsChamber();
```

This utility simplifies beamline setup and makes it easier to switch between realistic GDML-based geometries and fast approximations for testing.

---

## OpticalSimulation_BASE Geometry Construction

**File:**: `OpticalSimulationGeometryConstruction.cc`
[TO DO]

---

## Physics List
[TO DO & ADAPT]
**Class:** `OpticalSimulationPhysics`  
Derived from `G4VModularPhysicsList`

**Includes:**
- High-precision hadronic and elastic models
- Electromagnetic physics (`G4EmStandardPhysics_option3`)
- Stopping physics
- Decay and radioactive decay

---

## Step-Level Tracking and Data Extraction

**File:** `OpticalSimulationSteppingAction.cc`  
[TO DO & ADAPT]

The `OpticalSimulationSteppingAction` class handles fine-grained particle tracking at each simulation step in the PALLAS project. It collects particle data, manages quadrupole interactions, collimator events, and YAG screen statistics.

**Functionality:**

- Extracts positions, momentum directions, kinetic energy and deposited energy at each Geant4 step.
- Stores initial beam parameters for primary particles at the first step.
- Tracks particle crossings through quadrupoles and records entry/exit statistics.
- Updates collimator interactions: records positions and energies, optionally terminates tracks if collimator tracking is disabled.
- Updates YAG screen statistics: exit positions, deposited energy, particle IDs and total deposited energy.
- Automatically kills particles leaving the world volume.

**Purpose:**

- Provides detailed per-step data collection for beamline analysis.
- Enables statistics for quadrupoles, collimators and diagnostics screens.
- Integrates with `PlasmaMLPALLASEventAction` for storing and tallying simulation results.

---

## ROOT Output
[TO DO]

- Variables are initialized in `BeginOfEventAction` and filled in `EndOfEventAction`.
- Particle propagation and interaction info is collected in `PlasmaMLPALLASSteppingAction.cc`.

---

## Architecture Diagram
[TO DO]

---

## Documentation

La documentation du projet est disponible ici :  
[📖 Documentation Doxygen](https://ahuber33.github.io/PlasmaMLPALLAS/)


**Generate with:**

```bash
doxygen Doxyfile
```

**Access via:** `docs/html/index.html`

---

## License

GEANT4 Collaboration  

See the LICENSE file for details.