cmake_minimum_required (VERSION 3.5)
project (AimIO)

# === Versioning

set(AIMIO_MAJOR_VERSION 1)
set(AIMIO_MINOR_VERSION 0)
set(AIMIO_PATCH_VERSION 0)
set(AIMIO_VERSION
  ${AIMIO_MAJOR_VERSION}.${AIMIO_MINOR_VERSION}.${AIMIO_PATCH_VERSION})

# === Source code files

set (SRC
  source/AimIO.cxx
  source/Compression.cxx)

# === Testing

option (ENABLE_TESTING "Build unit and other tests" OFF)
if (ENABLE_TESTING)
    enable_testing()
    find_package (GTest REQUIRED)
endif (ENABLE_TESTING)

# == Dependencies

find_package (n88util 2.0 REQUIRED)
include_directories (${N88UTIL_INCLUDE_DIRS})
mark_as_advanced (n88util_DIR)

find_package (Boost 1.57.0 COMPONENTS filesystem system REQUIRED)
if (MSVC)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS)
endif (MSVC)
if (WIN32)
  # Disable auto-linking
  add_definitions (-DBOOST_ALL_NO_LIB)
endif()

include_directories (${Boost_INCLUDE_DIR})

# == Create library

include_directories (${CMAKE_BINARY_DIR})  # for aimio_export.h
include_directories (${CMAKE_SOURCE_DIR}/include)
include (GenerateExportHeader)

add_library (AimIO ${SRC})
set_target_properties(AimIO 
  PROPERTIES
    VERSION ${AIMIO_VERSION}
    SOVERSION ${AIMIO_MAJOR_VERSION}.${AIMIO_MINOR_VERSION})

generate_export_header (AimIO)

target_link_libraries (AimIO
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY})

# === Install

install (TARGETS AimIO
         EXPORT AimIOTargets
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib)

install (DIRECTORY "${CMAKE_SOURCE_DIR}/include/AimIO" DESTINATION include)
install(FILES ${PROJECT_BINARY_DIR}/aimio_export.h DESTINATION include/AimIO)

# === Testing

if (ENABLE_TESTING)
  add_subdirectory (tests)
endif (ENABLE_TESTING)

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
install(EXPORT AimIOTargets DESTINATION
  "${INSTALL_CMAKE_DIR}")
