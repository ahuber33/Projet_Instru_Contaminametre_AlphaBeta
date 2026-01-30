/**
 * @file OpticalSimulationMaterials.cc
 * @brief Implementation of the Materials class for the Optical
 * simulation.
 *
 * [TO DO]
 *
 * @author Arnaud HUBER <huber@lp2ib.in2p3.fr>
 * @date 2026
 */

#include "OpticalSimulationMaterials.hh"

using namespace CLHEP;

const G4String OpticalSimulationMaterials::path = "../simulation_input_files/";
std::mutex materialMutex;

OpticalSimulationMaterials *OpticalSimulationMaterials::fgInstance = nullptr;

OpticalSimulationMaterials::OpticalSimulationMaterials() : fMaterialsList{} {

    // #######################################################################################################################################
    // #######################################################################################################################################

    // ███╗░░░███╗██╗░░░██╗██╗░░░░░░█████╗░██████╗░
    // ████╗░████║╚██╗░██╔╝██║░░░░░██╔══██╗██╔══██╗
    // ██╔████╔██║░╚████╔╝░██║░░░░░███████║██████╔╝
    // ██║╚██╔╝██║░░╚██╔╝░░██║░░░░░██╔══██║██╔══██╗
    // ██║░╚═╝░██║░░░██║░░░███████╗██║░░██║██║░░██║
    // ╚═╝░░░░░╚═╝░░░╚═╝░░░╚══════╝╚═╝░░╚═╝╚═╝░░╚═╝

    auto mylar = G4NistManager::Instance()->FindOrBuildMaterial("G4_MYLAR");
    fMaterialsList.push_back(mylar);

    // #######################################################################################################################################
    // #######################################################################################################################################

    // ████████╗███████╗███████╗██╗░░░░░░█████╗░███╗░░██╗
    // ╚══██╔══╝██╔════╝██╔════╝██║░░░░░██╔══██╗████╗░██║
    // ░░░██║░░░█████╗░░█████╗░░██║░░░░░██║░░██║██╔██╗██║
    // ░░░██║░░░██╔══╝░░██╔══╝░░██║░░░░░██║░░██║██║╚████║
    // ░░░██║░░░███████╗██║░░░░░███████╗╚█████╔╝██║░╚███║
    // ░░░╚═╝░░░╚══════╝╚═╝░░░░░╚══════╝░╚════╝░╚═╝░░╚══╝

    auto teflon = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
    fMaterialsList.push_back(teflon);

    // #######################################################################################################################################
    // #######################################################################################################################################

    // ██╗░░░██╗░█████╗░░█████╗░██╗░░░██╗██╗░░░██╗███╗░░░███╗
    // ██║░░░██║██╔══██╗██╔══██╗██║░░░██║██║░░░██║████╗░████║
    // ╚██╗░██╔╝███████║██║░░╚═╝██║░░░██║██║░░░██║██╔████╔██║
    // ░╚████╔╝░██╔══██║██║░░██╗██║░░░██║██║░░░██║██║╚██╔╝██║
    // ░░╚██╔╝░░██║░░██║╚█████╔╝╚██████╔╝╚██████╔╝██║░╚═╝░██║
    // ░░░╚═╝░░░╚═╝░░╚═╝░╚════╝░░╚═════╝░░╚═════╝░╚═╝░░░░░╚═╝

    // Be careful of this vacuum definition.  This is only used to define
    // a refractive index so that the detector boundaries are defined.

    auto Vacuum = new G4Material("Vacuum", 1., 1. * g / mole, 1.e-20 * g / cm3,
                                 kStateGas, 0.1 * kelvin, 1.e-20 * bar);

    {
        Read.clear();
        Readabsorb.clear();
        Readindex.clear();
        Emission_Energy.clear();
        Emission_Ratio.clear();
        Absorption_Energy.clear();
        Absorption_Long.clear();
        Index_Energy.clear();
        Index_Value.clear();

        auto vacMPT = new G4MaterialPropertiesTable();

        G4double vacAbsorbconst = 10000 * m;

        file = path +
               "EJ-212.cfg"; // simply index filler values...not EJ-212 values

        Readabsorb.open(file);
        if (Readabsorb.is_open()) {
            while (!Readabsorb.eof()) {
                Readabsorb >> pWavelength >> filler >> var;
                Absorption_Energy.push_back((1240 / pWavelength) *
                                            eV); // convert wavelength to eV
                Absorption_Long.push_back(vacAbsorbconst);
                Index_Energy.push_back((1240 / pWavelength) *
                                       eV); // convert wavelength to eV
                Index_Value.push_back(1.0);
            }
        } else
            G4cout << "Error opening file: " << file << G4endl;
        Readabsorb.close();

        vacMPT->AddProperty("RINDEX", Index_Energy, Index_Value);
        vacMPT->AddProperty("ABSLENGTH", Absorption_Energy, Absorption_Long);
        Vacuum->SetMaterialPropertiesTable(vacMPT);
    }

    fMaterialsList.push_back(Vacuum);

    // #######################################################################################################################################
    // #######################################################################################################################################

    // ███████╗░█████╗░██╗░░██╗███████╗  ██╗░░░██╗░█████╗░░█████╗░██╗░░░██╗██╗░░░██╗███╗░░░███╗
    // ██╔════╝██╔══██╗██║░██╔╝██╔════╝  ██║░░░██║██╔══██╗██╔══██╗██║░░░██║██║░░░██║████╗░████║
    // █████╗░░███████║█████═╝░█████╗░░  ╚██╗░██╔╝███████║██║░░╚═╝██║░░░██║██║░░░██║██╔████╔██║
    // ██╔══╝░░██╔══██║██╔═██╗░██╔══╝░░  ░╚████╔╝░██╔══██║██║░░██╗██║░░░██║██║░░░██║██║╚██╔╝██║
    // ██║░░░░░██║░░██║██║░╚██╗███████╗  ░░╚██╔╝░░██║░░██║╚█████╔╝╚██████╔╝╚██████╔╝██║░╚═╝░██║
    // ╚═╝░░░░░╚═╝░░╚═╝╚═╝░░╚═╝╚══════╝  ░░░╚═╝░░░╚═╝░░╚═╝░╚════╝░░╚═════╝░░╚═════╝░╚═╝░░░░░╚═╝

    // Be careful of this vacuum definition.  This is only used to define
    // a refractive index so that the detector boundaries are defined.

    auto VacuumWorld =
        new G4Material("VacuumWorld", 1., 1. * g / mole, 1.e-20 * g / cm3,
                       kStateGas, 0.1 * kelvin, 1.e-20 * bar);

    fMaterialsList.push_back(VacuumWorld);

    // #######################################################################################################################################
    // #######################################################################################################################################

    // ░█████╗░██╗██████╗░
    // ██╔══██╗██║██╔══██╗
    // ███████║██║██████╔╝
    // ██╔══██║██║██╔══██╗
    // ██║░░██║██║██║░░██║
    // ╚═╝░░╚═╝╚═╝╚═╝░░╚═╝

    auto Air = new G4Material("Air", 1.290 * mg / cm3, 2);
    Air->AddElement(G4NistManager::Instance()->FindOrBuildElement("N"), 0.7);
    Air->AddElement(G4NistManager::Instance()->FindOrBuildElement("O"), 0.3);

    {
        Read.clear();
        Readabsorb.clear();
        Readindex.clear();
        Emission_Energy.clear();
        Emission_Ratio.clear();
        Absorption_Energy.clear();
        Absorption_Long.clear();
        Index_Energy.clear();
        Index_Value.clear();

        auto airMPT = new G4MaterialPropertiesTable();

        G4double vacAbsorbconst = 10000 * m;

        file = path +
               "EJ-212.cfg"; // simply index filler values...not EJ-212 values

        Readabsorb.open(file);
        if (Readabsorb.is_open()) {
            while (!Readabsorb.eof()) {
                Readabsorb >> pWavelength >> filler >> var;
                Absorption_Energy.push_back((1240 / pWavelength) *
                                            eV); // convert wavelength to eV
                Absorption_Long.push_back(var);
                Index_Energy.push_back((1240 / pWavelength) *
                                       eV); // convert wavelength to eV
                Index_Value.push_back(1.0);
            }
        } else
            G4cout << "Error opening file: " << file << G4endl;
        Readabsorb.close();

        airMPT->AddProperty("RINDEX", Index_Energy, Index_Value);
        airMPT->AddProperty("ABSLENGTH", Absorption_Energy, Absorption_Long);
        Air->SetMaterialPropertiesTable(airMPT);
    }
    fMaterialsList.push_back(Air);

    // #######################################################################################################################################
    // #######################################################################################################################################

    // ███████╗░░░░░██╗░░░░░░██████╗░░░███╗░░██████╗░
    // ██╔════╝░░░░░██║░░░░░░╚════██╗░████║░░╚════██╗
    // █████╗░░░░░░░██║█████╗░░███╔═╝██╔██║░░░░███╔═╝
    // ██╔══╝░░██╗░░██║╚════╝██╔══╝░░╚═╝██║░░██╔══╝░░
    // ███████╗╚█████╔╝░░░░░░███████╗███████╗███████╗
    // ╚══════╝░╚════╝░░░░░░░╚══════╝╚══════╝╚══════╝

    auto EJ212 =
        new G4Material("EJ212",
                       1.032 * g / cm3, // 1.053
                       2, kStateSolid, 273.15 * kelvin, 1.0 * atmosphere);

    EJ212->AddElement(G4NistManager::Instance()->FindOrBuildElement("H"), 10);
    EJ212->AddElement(G4NistManager::Instance()->FindOrBuildElement("C"), 9);

    {
        Read.clear();
        Readabsorb.clear();
        Readindex.clear();
        Emission_Energy.clear();
        Emission_Ratio.clear();
        Absorption_Energy.clear();
        Absorption_Long.clear();
        Index_Energy.clear();
        Index_Value.clear();

        auto EJ212MPT = new G4MaterialPropertiesTable();

        // Read primary emission spectrum
        file = path + "EJ-212.cfg";

        Read.open(file);
        if (Read.is_open()) {
            while (!Read.eof()) {
                Read >> pWavelength >> filler >> var;
                // G4cout << "Wavelength = " << 1240./pWavelength << " &
                // emission = "<< var << G4endl;
                Emission_Energy.push_back((1240. / pWavelength) *
                                          eV); // convert wavelength to eV
                Emission_Ratio.push_back(var);
            }
        } else {
            G4cout << "Error opening file: " << file << G4endl;
        }
        Read.close();

        // // Read primary bulk absorption

        file = path + "PSTBulkAbsorb_reverse.cfg";

        Readabsorb.open(file);
        if (Readabsorb.is_open()) {
            while (!Readabsorb.eof()) {
                Readabsorb >> pWavelength >> filler >> var;
                // G4cout << "Wavelength = " << pWavelength << " & absorption =
                // "<< varabsorblength << G4endl;
                Absorption_Energy.push_back((1240. / pWavelength) * eV);
                Absorption_Long.push_back(1. * var * m);
            }
        } else

            G4cout << "Error opening file: " << file << G4endl;

        Readabsorb.close();

        // Read WLS absorption
        //
        // wlsAbEntries = 0;
        // std::ifstream ReadWLSa;
        // G4String WLSabsorb = path+"UPS923.cfg";
        //
        // ReadWLSa.open(WLSabsorb);
        // if (ReadWLSa.is_open()){
        //  while(!ReadWLSa.eof()){
        // 	 G4String filler;
        // 	 ReadWLSa>>pWavelength>>filler>>wlsabsorblength;
        // 	 wlsEnergy[wlsAbEntries] = (1240/pWavelength)*eV;
        //
        //
        // 	 if (wlsAbEntries < 200){
        // wlsAbsorb[wlsAbEntries] = wlsabsorblength*m;
        // 	 }
        // 	 else{
        // wlsAbsorb[wlsAbEntries] = wlsabsorblength*m;
        // 	 }
        //
        // 	 wlsAbEntries++;
        //  }
        // }
        // else
        //  {
        // 	 G4cout << "Error opening file: " << WLSabsorb << G4endl;
        //  }
        //
        // ReadWLSa.close();

        // Read WLS emission
        // wlsEmEntries = 0;
        // std::ifstream ReadWLSe;
        // G4String WLSemit = path+"full_popop_emission.cfg";
        // ReadWLSe.open(WLSemit);
        // if(ReadWLSe.is_open()){
        //  while(!ReadWLSe.eof()){
        // 	 G4String filler;
        // 	 ReadWLSe >> pWavelength >> filler >> wlsEmit[wlsEmEntries];
        // 	 wlsEnergy[wlsEmEntries] = (1240/pWavelength)*eV;
        // 	 wlsEmEntries++;
        //  }
        // }
        // else
        //  G4cout << "Error opening file: " << WLSemit << G4endl;
        // ReadWLSe.close();

        // Read scintillator refractive index

        // G4String ref_index_emit = path+"PST_ref_index.dat";
        file = path + "PS_index_geant_reverse.cfg";

        Readindex.open(file);
        if (Readindex.is_open()) {
            while (!Readindex.eof()) {
                Readindex >> pWavelength >> filler >> var;
                // ref_index_value[ref_index_Entries]=1.59;
                Index_Energy.push_back((1240 / pWavelength) * eV);
                Index_Value.push_back(var);
                // EJ212_Index_Value.push_back(1.59);
            }
        } else
            G4cout << "Error opening file: " << file << G4endl;
        Readindex.close();

        // Now apply the properties table

        // scintMPT->AddProperty("WLSCOMPONENT",wlsEnergy,wlsEmit,wlsEmEntries);
        // scintMPT->AddProperty("WLSABSLENGTH",wlsEnergy,wlsAbsorb,wlsAbEntries);
        // // the WLS absorption spectrum
        // scintMPT->AddConstProperty("WLSTIMECONSTANT",12*ns);
        EJ212MPT->AddProperty("RINDEX", Index_Energy, Index_Value);

        EJ212MPT->AddProperty("ABSLENGTH", Absorption_Energy,
                              Absorption_Long); // the bulk absorption spectrum
        EJ212MPT->AddProperty("SCINTILLATIONCOMPONENT1", Emission_Energy,
                              Emission_Ratio);
        // scintMPT->AddProperty("SCINTILLATIONCOMPONENT1",scintEnergy,scintEmit,scintEntries);
        // scintMPT->AddProperty("SCINTILLATIONCOMPONENT2",scintEnergy,scintEmit,scintEntries);
        // // if slow component

        // G4double efficiency = 1.0;
        // scintMPT->AddConstProperty("EFFICIENCY",efficiency);

        EJ212MPT->AddConstProperty("SCINTILLATIONYIELD", 10000 / MeV);
        // scintMPT->AddConstProperty("ALPHASCINTILLATIONYIELD",0.01*lightyield/MeV);
        Res = 1;
        EJ212MPT->AddConstProperty("RESOLUTIONSCALE", Res);
        Fastconst = 2.1 * ns;
        EJ212MPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", Fastconst);
        Slowconst = 10 * ns;
        EJ212MPT->AddConstProperty("SCINTILLATIONTIMECONSTANT2",
                                   Slowconst); // if slow component
        EJ212MPT->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
        EJ212MPT->AddConstProperty("SCINTILLATIONYIELD2", 0.0);

        EJ212->SetMaterialPropertiesTable(EJ212MPT);
        // scintillator->GetIonisation()->SetBirksConstant(0.0872*mm/MeV);
        // //0.126->base; 0.0872->article BiPO
        // scintillator->GetIonisation()->SetBirksConstant(0.25*mm/MeV); //
        // Choisi pour validation modÃ¨le avec LY 11737!!!
        // scintillator->GetIonisation()->SetBirksConstant(0.22*mm/MeV);
        // scintillator->GetIonisation()->SetBirksConstant(0.01*mm/MeV); // TEST
        // ELECTRONS !!! => Maxime
    }

    // printMaterialProperties(EJ212);
    fMaterialsList.push_back(EJ212);

    // #######################################################################################################################################
    // #######################################################################################################################################

    // ██████╗░░█████╗░██████╗░░█████╗░░██████╗██╗██╗░░░░░██╗░█████╗░░█████╗░████████╗███████╗
    // ██╔══██╗██╔══██╗██╔══██╗██╔══██╗██╔════╝██║██║░░░░░██║██╔══██╗██╔══██╗╚══██╔══╝██╔════╝
    // ██████╦╝██║░░██║██████╔╝██║░░██║╚█████╗░██║██║░░░░░██║██║░░╚═╝███████║░░░██║░░░█████╗░░
    // ██╔══██╗██║░░██║██╔══██╗██║░░██║░╚═══██╗██║██║░░░░░██║██║░░██╗██╔══██║░░░██║░░░██╔══╝░░
    // ██████╦╝╚█████╔╝██║░░██║╚█████╔╝██████╔╝██║███████╗██║╚█████╔╝██║░░██║░░░██║░░░███████╗
    // ╚═════╝░░╚════╝░╚═╝░░╚═╝░╚════╝░╚═════╝░╚═╝╚══════╝╚═╝░╚════╝░╚═╝░░╚═╝░░░╚═╝░░░╚══════╝

    // ░██████╗░██╗░░░░░░█████╗░░██████╗░██████╗
    // ██╔════╝░██║░░░░░██╔══██╗██╔════╝██╔════╝
    // ██║░░██╗░██║░░░░░███████║╚█████╗░╚█████╗░
    // ██║░░╚██╗██║░░░░░██╔══██║░╚═══██╗░╚═══██╗
    // ╚██████╔╝███████╗██║░░██║██████╔╝██████╔╝
    // ░╚═════╝░╚══════╝╚═╝░░╚═╝╚═════╝░╚═════╝░

    // GLASS PMT
    //  Silicon Dioxide
    auto SiO2 =
        G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");

    // DiBoron TriOxide
    auto B2O3 =
        G4NistManager::Instance()->FindOrBuildMaterial("G4_BORON_OXIDE");

    auto bs_glass = new G4Material("bs_glass", 2.23 * g / cm3, 2, kStateSolid,
                                   273.15 * kelvin, 1.0 * atmosphere);

    bs_glass->AddMaterial(SiO2, 0.9);
    bs_glass->AddMaterial(B2O3, 0.1);

    {
        Read.clear();
        Readabsorb.clear();
        Readindex.clear();
        Emission_Energy.clear();
        Emission_Ratio.clear();
        Absorption_Energy.clear();
        Absorption_Long.clear();
        Index_Energy.clear();
        Index_Value.clear();

        auto bs_glassMPT = new G4MaterialPropertiesTable();

        file = path + "Borosilicate_GlassBulkAbsorb_reverse.cfg";

        Readabsorb.open(file);

        if (Readabsorb.is_open()) {
            while (!Readabsorb.eof()) {
                Readabsorb >> pWavelength >> filler >> var;
                Absorption_Energy.push_back((1240 / pWavelength) * eV);
                Absorption_Long.push_back(var * m);
            }
        } else
            G4cout << "Error opening file: " << file << G4endl;

        Readabsorb.close();

        file = path + "BSG_ref_index_reverse.dat";

        Readindex.open(file);
        Readindex.clear();
        if (Readindex.is_open()) {
            while (!Readindex.eof()) {
                Readindex >> pWavelength >> filler >> var;
                Index_Energy.push_back((1240 / pWavelength) * eV);
                Index_Value.push_back(var);
                //      G4cout << " Energy = " <<
                //      bsgindexEnergy[bsgindexEntries] << "    Index = " <<
                //      bsgindexvalue[bsgindexEntries] << G4endl;
            }
        }

        else
            G4cout << "Error opening file: " << file << G4endl;

        Readindex.close();

        bs_glassMPT->AddProperty("ABSLENGTH", Absorption_Energy,
                                 Absorption_Long);
        bs_glassMPT->AddProperty("RINDEX", Index_Energy, Index_Value);
        bs_glass->SetMaterialPropertiesTable(bs_glassMPT);
    }

    fMaterialsList.push_back(bs_glass);

    // #######################################################################################################################################
    // #######################################################################################################################################

    // ░█████╗░░█████╗░██████╗░░██████╗░██╗██╗░░░░░██╗░░░░░███████╗
    // ██╔══██╗██╔══██╗██╔══██╗██╔════╝░██║██║░░░░░██║░░░░░██╔════╝
    // ██║░░╚═╝███████║██████╔╝██║░░██╗░██║██║░░░░░██║░░░░░█████╗░░
    // ██║░░██╗██╔══██║██╔══██╗██║░░╚██╗██║██║░░░░░██║░░░░░██╔══╝░░
    // ╚█████╔╝██║░░██║██║░░██║╚██████╔╝██║███████╗███████╗███████╗
    // ░╚════╝░╚═╝░░╚═╝╚═╝░░╚═╝░╚═════╝░╚═╝╚══════╝╚══════╝╚══════╝

    auto cargille = new G4Material("cargille", 0.99 * g / cm3, 4, kStateSolid,
                                   273.15 * kelvin, 1.0 * atmosphere);

    cargille->AddElement(G4NistManager::Instance()->FindOrBuildElement("H"), 6);
    cargille->AddElement(G4NistManager::Instance()->FindOrBuildElement("C"), 2);
    cargille->AddElement(G4NistManager::Instance()->FindOrBuildElement("O"), 1);
    cargille->AddElement(G4NistManager::Instance()->FindOrBuildElement("Si"),
                         1);

    {
        Read.clear();
        Readabsorb.clear();
        Readindex.clear();
        Emission_Energy.clear();
        Emission_Ratio.clear();
        Absorption_Energy.clear();
        Absorption_Long.clear();
        Index_Energy.clear();
        Index_Value.clear();
        G4double cargille_absorblength;
        G4double cargilleIndexconst = 1.406; // 1.49 1.406 RTV

        auto cargilleMPT = new G4MaterialPropertiesTable();

        file = path + "CargilleBulkAbsorb_reverse.cfg";

        Readabsorb.open(file);
        if (Readabsorb.is_open()) {
            while (!Readabsorb.eof()) {
                Readabsorb >> pWavelength >> filler >> var;
                Absorption_Energy.push_back((1240. / pWavelength) * eV);
                Absorption_Long.push_back(var * m);
                Index_Energy.push_back((1240. / pWavelength) * eV);
                Index_Value.push_back(cargilleIndexconst);
            }
        } else
            G4cout << "Error opening file: " << file << G4endl;
        Readabsorb.close();

        cargilleMPT->AddProperty("RINDEX", Index_Energy, Index_Value);
        cargilleMPT->AddProperty("ABSLENGTH", Absorption_Energy,
                                 Absorption_Long);
        cargille->SetMaterialPropertiesTable(cargilleMPT);
    }

    fMaterialsList.push_back(cargille);

    // #######################################################################################################################################
    // #######################################################################################################################################

    // ██████╗░███╗░░░███╗███╗░░░███╗░█████╗░
    // ██╔══██╗████╗░████║████╗░████║██╔══██╗
    // ██████╔╝██╔████╔██║██╔████╔██║███████║
    // ██╔═══╝░██║╚██╔╝██║██║╚██╔╝██║██╔══██║
    // ██║░░░░░██║░╚═╝░██║██║░╚═╝░██║██║░░██║
    // ╚═╝░░░░░╚═╝░░░░░╚═╝╚═╝░░░░░╚═╝╚═╝░░╚═╝

    // PMMA
    auto PMMA = new G4Material("PMMA", 1.19 * g / cm3, 3, kStateSolid,
                               273.15 * kelvin, 1.0 * atmosphere);
    PMMA->AddElement(G4NistManager::Instance()->FindOrBuildElement("H"), 0.532);
    PMMA->AddElement(G4NistManager::Instance()->FindOrBuildElement("C"), 0.336);
    PMMA->AddElement(G4NistManager::Instance()->FindOrBuildElement("H"), 0.132);

    {
        Read.clear();
        Readabsorb.clear();
        Readindex.clear();
        Emission_Energy.clear();
        Emission_Ratio.clear();
        Absorption_Energy.clear();
        Absorption_Long.clear();
        Index_Energy.clear();
        Index_Value.clear();

        auto PMMAMPT = new G4MaterialPropertiesTable();

        file = path + "PMMA_ref_index_geant_reverse.dat";

        Readindex.open(file);
        if (Readindex.is_open()) {
            while (!Readindex.eof()) {
                Readindex >> pWavelength >> filler >> var;
                Index_Energy.push_back((1240 / pWavelength) * eV);
                // Index_Value.push_back(var);
                Index_Value.push_back(1.49);
            }
        } else
            G4cout << "Error opening file: " << file << G4endl;
        Readindex.close();

        file = path + "PMMABulkAbsorb_reverse.dat";

        //  Read_pmma_Bulk.open(pmma_Bulk);
        Readabsorb.open(file);

        if (Readabsorb.is_open()) {
            while (!Readabsorb.eof()) {
                Readabsorb >> pWavelength >> filler >> var;
                Absorption_Energy.push_back((1240 / pWavelength) * eV);
                Absorption_Long.push_back(var * m);
            }
        } else
            //    G4cout << "Error opening file: " << Bulk << G4endl;
            G4cout << "Error opening file: " << file << G4endl;

        Readabsorb.close();

        PMMAMPT->AddProperty("ABSLENGTH", Absorption_Energy, Absorption_Long);
        PMMAMPT->AddProperty("RINDEX", Index_Energy, Index_Value);

        PMMA->SetMaterialPropertiesTable(PMMAMPT);
    }

    fMaterialsList.push_back(PMMA);

    // #######################################################################################################################################
    // #######################################################################################################################################

    // ███████╗███╗░░██╗░██████╗██╗░█████╗░░██████╗░
    // ╚════██║████╗░██║██╔════╝╚═╝██╔══██╗██╔════╝░
    // ░░███╔═╝██╔██╗██║╚█████╗░░░░███████║██║░░██╗░
    // ██╔══╝░░██║╚████║░╚═══██╗░░░██╔══██║██║░░╚██╗
    // ███████╗██║░╚███║██████╔╝██╗██║░░██║╚██████╔╝
    // ╚══════╝╚═╝░░╚══╝╚═════╝░╚═╝╚═╝░░╚═╝░╚═════╝░

    auto ZnS = new G4Material("ZnS", 4.1 * g / cm3, 2);
    ZnS->AddElement(G4NistManager::Instance()->FindOrBuildElement("Zn"), 1);
    ZnS->AddElement(G4NistManager::Instance()->FindOrBuildElement("S"), 1);

    {
        Read.clear();
        Readabsorb.clear();
        Readindex.clear();
        Emission_Energy.clear();
        Emission_Ratio.clear();
        Absorption_Energy.clear();
        Absorption_Long.clear();
        Index_Energy.clear();
        Index_Value.clear();

        auto ZnSMPT = new G4MaterialPropertiesTable();

        // Read primary emission spectrum

        file = path + "ZnS_spectrum.dat";

        Read.open(file);
        if (Read.is_open()) {
            while (!Read.eof()) {
                Read >> pWavelength >> filler >> var;
                // G4cout << "Wavelength = " << 1240./pWavelength << " &
                // emission = "<< ratio << G4endl;
                Emission_Energy.push_back((1240. / pWavelength) *
                                          eV); // convert wavelength to eV
                Emission_Ratio.push_back(var);
            }
        } else {
            G4cout << "Error opening file: " << file << G4endl;
        }
        Read.close();

        // Read primary bulk absorption

        file = path + "LaBr3_absorption_reverse.cfg";

        Readabsorb.open(file);
        if (Readabsorb.is_open()) {
            while (!Readabsorb.eof()) {
                Readabsorb >> pWavelength >> filler >> var;
                // G4cout << "Wavelength = " << pWavelength << " & absorption =
                // "<< varabsorblength << G4endl;
                Absorption_Energy.push_back((1240. / pWavelength) * eV);
                Absorption_Long.push_back(0.15 * mm);
            }
        } else

            G4cout << "Error opening file: " << file << G4endl;

        Readabsorb.close();

        file = path + "ZnS_index_reverse.cfg";

        Readindex.open(file);
        if (Readindex.is_open()) {
            while (!Readindex.eof()) {
                Readindex >> pWavelength >> filler >> var;
                Index_Energy.push_back((1240 / pWavelength) * eV);
                Index_Value.push_back(var);
            }
        } else
            G4cout << "Error opening file: " << file << G4endl;
        Readindex.close();

        // Now apply the properties table
        ZnSMPT->AddProperty("RINDEX", Index_Energy, Index_Value);
        ZnSMPT->AddProperty("ABSLENGTH", Absorption_Energy,
                            Absorption_Long); // the bulk absorption spectrum
        ZnSMPT->AddProperty("SCINTILLATIONCOMPONENT1", Emission_Energy,
                            Emission_Ratio);
        // scintMPT->AddProperty("SCINTILLATIONCOMPONENT2",scintEnergy,scintEmit,scintEntries);
        // // if slow component

        ZnSMPT->AddConstProperty("SCINTILLATIONYIELD", lightyieldZnS / MeV);
        Res = 1;
        ZnSMPT->AddConstProperty("RESOLUTIONSCALE", Res);
        Fastconst = 200 * ns;
        ZnSMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", Fastconst);
        Slowconst = 1000 * ns;
        ZnSMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT2",
                                 Slowconst); // if slow component
        ZnSMPT->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
        ZnSMPT->AddConstProperty("SCINTILLATIONYIELD2", 0.0);

        ZnS->SetMaterialPropertiesTable(ZnSMPT);
    }

    fMaterialsList.push_back(ZnS);
    // #######################################################################################################################################
    // #######################################################################################################################################
}

OpticalSimulationMaterials::~OpticalSimulationMaterials() {}

G4Material *OpticalSimulationMaterials::getMaterial(const char *materialId) {
    for (int i = 0; i < (int)fMaterialsList.size(); i++) {
        if (fMaterialsList[i]->GetName() == materialId) {
            G4cout << "Material : " << materialId << " found" << G4endl;
            return fMaterialsList[i];
        }
    }
    G4cout << "ERROR: Materials::getMaterial material " << materialId
           << " not found." << G4endl;
    return NULL;
}

void OpticalSimulationMaterials::printMaterialProperties(G4Material *material) {
    std::cout << "\nMaterial name: " << material->GetName() << std::endl;
    G4MaterialPropertiesTable *mpt = material->GetMaterialPropertiesTable();
    if (!mpt)
        return;

    std::vector<G4double> fOpticalPhotonWavelength = {
        360, 400., 440., 480., 520., 560., 600., 640., 680., 720., 760};

    // Print photon wavelengths
    std::cout << std::left << std::setw(TAB_COLUMN_1) << "PHOTON_WAVELEGTH, nm";
    for (G4int i = 0; i < (G4int)fOpticalPhotonWavelength.size(); i++) {
        std::cout << std::left << std::setw(TAB_COLUMN)
                  << fOpticalPhotonWavelength[i];
    }

    std::cout << std::endl;

    // Print photon energy
    std::cout << std::left << std::setw(TAB_COLUMN_1) << "PHOTON_ENERGY, eV";
    for (G4int i = 0; i < (G4int)fOpticalPhotonWavelength.size(); i++) {
        std::cout << std::left << std::setw(TAB_COLUMN)
                  << wavelengthNmToEnergy(fOpticalPhotonWavelength[i]) / eV;
    }
    std::cout << std::endl;

    // Print material properties
    std::vector<G4String> propertyNames = mpt->GetMaterialPropertyNames();

    // Replaced in Geant4 v.11 with const
    // std::vector<G4MaterialPropertyVector*>& GetProperties() const const
    // std::map<G4int, G4MaterialPropertyVector*, std::less<G4int> >* pMapNew =
    // mpt->GetPropertyMap();
    std::vector<G4String> materialPropertiesNames =
        mpt->GetMaterialPropertyNames();

    for (G4String propertyName : materialPropertiesNames) {
        G4MaterialPropertyVector *property = mpt->GetProperty(propertyName);
        if (!property)
            continue;

        // Print property name
        std::cout << std::left << std::setw(TAB_COLUMN_1) << propertyName;
        // Print property values
        for (G4int i = 0; i < (G4int)fOpticalPhotonWavelength.size(); i++) {
            G4bool b;
            G4double value = property->GetValue(
                wavelengthNmToEnergy(fOpticalPhotonWavelength[i]), b);
            std::cout << std::left << std::setw(TAB_COLUMN) << value;
        }

        std::cout << std::endl;
    }

    // Print material constant properties
    std::vector<G4String> constPropertyNames =
        mpt->GetMaterialConstPropertyNames();
    for (G4String constPropertyName : constPropertyNames) {
        if (!mpt->ConstPropertyExists(constPropertyName))
            continue;
        // Print property name and value
        G4double constPropertyValue = mpt->GetConstProperty(constPropertyName);
        std::cout << std::left << std::setw(TAB_COLUMN_1) << constPropertyName
                  << constPropertyValue << std::endl;
    }
}

void OpticalSimulationMaterials::printMaterialProperties(
    const char *materialId) {
    G4Material *material = getMaterial(materialId);
    if (material == NULL)
        return;
    printMaterialProperties(material);
}

OpticalSimulationMaterials *OpticalSimulationMaterials::getInstance() {
    static OpticalSimulationMaterials materials;
    if (fgInstance == nullptr) {
        fgInstance = &materials;
    }
    return fgInstance;
}

G4double OpticalSimulationMaterials::wavelengthNmToEnergy(G4double wavelength) {
    G4double hc = 1239.84193;
    return hc / wavelength * eV; // E (eV) = 1239.8 / l (nm)
}