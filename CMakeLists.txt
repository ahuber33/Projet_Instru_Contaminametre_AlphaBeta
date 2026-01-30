#----------------------------------------------------------------------------
# Setup the project
#----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.16...3.21)
project(OpticalSimulation)

#----------------------------------------------------------------------------
# Options
#----------------------------------------------------------------------------
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
option(GEANT4_USE_QT ON)
option(BUILD_TESTS "Build unit tests" ON)

#----------------------------------------------------------------------------
# Find Geant4 package
#----------------------------------------------------------------------------
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED gdml ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# ROOT
#----------------------------------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# Include directories
#----------------------------------------------------------------------------
include(${Geant4_USE_FILE})
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${Geant4_INCLUDE_DIR}
    ${ROOT_INCLUDE_DIRS}
)

#----------------------------------------------------------------------------
# Project sources and headers
#----------------------------------------------------------------------------
set(PROJECT_SRC
    src/OpticalSimulationPhysics.cc
    src/Geometry.cc
    src/OpticalSimulationEventAction.cc
    src/OpticalSimulationVisManager.cc
    src/OpticalSimulationGeometryMessenger.cc
    src/OpticalSimulationGeometryConstruction.cc
    src/OpticalSimulationPrimaryGeneratorAction.cc
    src/OpticalSimulationRunAction.cc
    src/OpticalSimulationSteppingAction.cc
    src/OpticalSimulationActionInitialization.cc
    src/OpticalSimulationMaterials.cc
)

set(PROJECT_HEADER
    include/OpticalSimulationPhysics.hh
    include/Geometry.hh
    include/OpticalSimulationEventAction.hh
    include/OpticalSimulationVisManager.hh
    include/OpticalSimulationGeometryMessenger.hh
    include/OpticalSimulationGeometryConstruction.hh
    include/OpticalSimulationPrimaryGeneratorAction.hh
    include/OpticalSimulationRunAction.hh
    include/OpticalSimulationSteppingAction.hh
    include/OpticalSimulationActionInitialization.hh
    include/OpticalSimulationMaterials.hh
)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(OpticalSimulation OpticalSimulation.cc ${PROJECT_HEADER} ${PROJECT_SRC})
#
target_link_libraries(OpticalSimulation ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/bin)
install(TARGETS OpticalSimulation DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
message("Directory :" ${CMAKE_CURRENT_SOURCE_DIR}/bin)