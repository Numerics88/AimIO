cmake_minimum_required (VERSION 3.20) # Bumped from 3.3
project (AimIO)

# === Versioning

set(AIMIO_MAJOR_VERSION 2)
set(AIMIO_MINOR_VERSION 0)
set(AIMIO_PATCH_VERSION 0)
set(AIMIO_VERSION
  ${AIMIO_MAJOR_VERSION}.${AIMIO_MINOR_VERSION}.${AIMIO_PATCH_VERSION})

# === Standard CMake options

option (BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Ensure that CMake behaves predictably
set (CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
set (CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY ON)
set (CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY ON)

# === Source code files

set (SRC
  source/AimIO.cxx
  source/IsqIO.cxx
  source/DateTime.cxx
  source/Compression.cxx)

# == Dependencies

find_package (n88util REQUIRED)
include_directories (${N88UTIL_INCLUDE_DIRS})
mark_as_advanced (n88util_DIR)

find_package (Boost
              COMPONENTS filesystem system thread
              CONFIG REQUIRED)
if (MSVC)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS)
endif (MSVC)
if (WIN32)
  # Disable auto-linking
  add_definitions (-DBOOST_ALL_NO_LIB)
endif()

include_directories (${Boost_INCLUDE_DIRS})

# == Create library

include_directories (${CMAKE_BINARY_DIR})  # for aimio_export.h
include_directories (${CMAKE_SOURCE_DIR}/include)
include (GenerateExportHeader)

add_library (AimIO ${SRC})
set_target_properties(AimIO 
  PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    VERSION ${AIMIO_VERSION}
    SOVERSION ${AIMIO_MAJOR_VERSION}.${AIMIO_MINOR_VERSION})

generate_export_header (AimIO)

target_link_libraries (AimIO
  PRIVATE
    n88util::n88util
    Boost::filesystem Boost::system
)

option (N88_BUILD_AIX "Build aix tool." ON)
if (N88_BUILD_AIX)
    add_executable (aix source/aix.cxx)
    target_link_libraries (aix
      AimIO
      Boost::filesystem Boost::system
    )
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_libraries (aix dl pthread)
    endif()
endif()

option (N88_BUILD_CTHEADER "Build ctheader tool." ON)
if (N88_BUILD_CTHEADER)
    add_executable (ctheader source/ctheader.cxx)
    target_link_libraries (ctheader
      AimIO
      Boost::filesystem Boost::system
    )
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_libraries (ctheader dl pthread)
    endif()
endif()

# === Install

install (TARGETS AimIO
         EXPORT AimIOTargets
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib)

if (N88_BUILD_AIX)
    install (TARGETS aix DESTINATION bin)
endif ()

if (N88_BUILD_CTHEADER)
    install (TARGETS ctheader DESTINATION bin)
endif ()


install (DIRECTORY "${CMAKE_SOURCE_DIR}/include/AimIO" DESTINATION include)
install(FILES ${PROJECT_BINARY_DIR}/aimio_export.h DESTINATION include/AimIO)

# === CMake config files for importing this project

# Set some locations for the install
set(INSTALL_LIB_DIR lib)
set(INSTALL_BIN_DIR bin)
set(INSTALL_INCLUDE_DIR include)
if(WIN32 AND NOT CYGWIN)
    set(INSTALL_CMAKE_DIR CMake)
else()
    set(INSTALL_CMAKE_DIR "lib/cmake/AimIO-${AIMIO_MAJOR_VERSION}.${AIMIO_MINOR_VERSION}")
endif()

# Make relative paths absolute
foreach(p LIB BIN INCLUDE CMAKE)
    set(var INSTALL_${p}_DIR)
    if(NOT IS_ABSOLUTE "${${var}}")
      set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()

# Add all targets to the build-tree export set
export (TARGETS AimIO
    NAMESPACE AimIO::
    FILE "${PROJECT_BINARY_DIR}/AimIOTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE AimIO)

# Create the AimIOConfig.cmake and AimIOConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}")
configure_file(AimIOConfig.cmake.in
    "${PROJECT_BINARY_DIR}/AimIOConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${AIMIO_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(AimIOConfig.cmake.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/AimIOConfig.cmake" @ONLY)
# ... for both
configure_file(AimIOConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/AimIOConfigVersion.cmake" @ONLY)

# Install the AimIOConfig.cmake and AimIOConfigVersion.cmake
install(FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/AimIOConfig.cmake"
    "${PROJECT_BINARY_DIR}/AimIOConfigVersion.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}")

# Install the export set for use with the install-tree
install(EXPORT AimIOTargets
  NAMESPACE
    AimIO::
  DESTINATION
    "${INSTALL_CMAKE_DIR}"
)

# === Testing

option (ENABLE_TESTING "Build unit and other tests" OFF)
if (ENABLE_TESTING)
  enable_testing()
  add_subdirectory (tests)
endif (ENABLE_TESTING)
