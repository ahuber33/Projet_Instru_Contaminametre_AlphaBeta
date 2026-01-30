#ifndef OpticalSimulationMaterials_h
#define OpticalSimulationMaterials_h

/**
 * @class OpticalSimulationMaterials
 * @brief Describe optical simulation materials
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 *
 * Provides UI commands to setup detector and readout geometry (prior to
 * initialization). Length, distance, gradients and display can be changed.
 */

#include "CLHEP/Units/SystemOfUnits.h"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4MaterialPropertyVector.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include <fstream>
#include <iostream>

class G4Material;

class OpticalSimulationMaterials {
  public:
    static OpticalSimulationMaterials *getInstance();
    virtual ~OpticalSimulationMaterials();

    G4Material *getMaterial(const char *);
    void printMaterialProperties(const char *);
    void printMaterialProperties(G4Material *material);

  protected:
    OpticalSimulationMaterials();

  private:
    static const G4String path;

    std::vector<G4Material *> fMaterialsList;
    static OpticalSimulationMaterials *fgInstance;
    G4double wavelengthNmToEnergy(G4double wavelength);
    G4double energyToWavelangthNm(G4double energy);
    const G4int TAB_COLUMN_1 = 25;
    const G4int TAB_COLUMN = 10;

    G4double var;
    G4double pWavelength;
    G4String filler;
    G4String file;
    std::ifstream Read;
    std::ifstream Readabsorb;
    std::ifstream Readindex;
    std::ifstream Readscatt;
    std::vector<G4double> Emission_Energy;
    std::vector<G4double> Emission_Ratio;
    std::vector<G4double> Absorption_Energy;
    std::vector<G4double> Absorption_Long;
    std::vector<G4double> Index_Energy;
    std::vector<G4double> Index_Value;
    std::vector<G4double> Scatt_Energy;
    std::vector<G4double> Scatt_Long;

    G4double Res = 0.0;
    G4double Fastconst = 0.0;
    G4double Slowconst = 0.0;

    G4double lightyield = 0.0;
    G4double lightyieldZnS = 0.0;
};
#endif
