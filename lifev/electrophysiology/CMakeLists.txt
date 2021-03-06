INCLUDE(TribitsPackageMacros)
INCLUDE(TribitsAddOptionAndDefine)
INCLUDE(TribitsLibraryMacros)

#
# A) Define the package
#

TRIBITS_PACKAGE(Electrophysiology)
SET(${PACKAGE_NAME}_VERSION "3.1")

#
# B) Set up package-specific options
#

TRIBITS_ADD_SHOW_DEPRECATED_WARNINGS_OPTION()

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

SET(HEADERS "")
SET(SOURCES "")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_SUBDIRECTORY(util)
SET(HEADERS ${HEADERS} ${util_HEADERS})
SET(SOURCES ${SOURCES} ${util_SOURCES})

ADD_SUBDIRECTORY(stimulus)
SET(HEADERS ${HEADERS} ${stimulus_HEADERS})
SET(SOURCES ${SOURCES} ${stimulus_SOURCES})


ADD_SUBDIRECTORY(solver)
SET(HEADERS ${HEADERS} ${solver_HEADERS} ${solver_IonicModels_HEADERS})
SET(SOURCES ${SOURCES} ${solver_IonicModels_SOURCES})

TRIBITS_EXCLUDE_AUTOTOOLS_FILES()
# This is the list of files excluded from the tarball
TRIBITS_EXCLUDE_FILES()

# Add library target
TRIBITS_ADD_LIBRARY(
  lifevelectrophysiology
  HEADERS ${HEADERS}
  SOURCES ${SOURCES}
)

TRIBITS_ADD_TEST_DIRECTORIES(testsuite)
TRIBITS_ADD_EXAMPLE_DIRECTORIES(examples)
TRIBITS_ADD_TEST_DIRECTORIES(unit)
#
# Do standard postprocessing
#
TRIBITS_PACKAGE_POSTPROCESS()
