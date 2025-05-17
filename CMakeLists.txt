#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(B4d)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB macros ${PROJECT_SOURCE_DIR}/macro/*.mac)
#---------------------------------------------------------------------------

#file(GLOB MACRO_FILES "*.mac" )
#file(COPY ${MACRO_FILES} DESTINATION ${PROJECT_BINARY_DIR})

#--------------------------------------------------------------------------
# Modificato per aggiungere le librerie di ROOT

#
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO)
if (ROOT_FOUND)
  message("ROOT found: ${ROOT_INCLUDE_DIRS}")
else()
  message (FATAL_ERROR "ROOT not found")
endif()
include_directories(${ROOT_INCLUDE_DIRS})
#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(NaI exampleB4d.cc ${sources} ${headers} ${macros})
# target_link_libraries(NaI ${Geant4_LIBRARIES})

target_link_libraries(NaI ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B4c. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(EXAMPLEB4D_SCRIPTS
  /in_out/exampleB4d.out
  /in_out/exampleB4.in
  /macro/gui.mac
  /macro/init_vis.mac
  /file.C/plotHisto.C
  /file.C/plotNtuple.C
  /macro/run1.mac
  /macro/run2.mac
  /macro/vis2.mac
  )

foreach(_script ${EXAMPLEB4D_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS NaI DESTINATION bin)
