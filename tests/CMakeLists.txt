find_package (GTest REQUIRED)

# === Fetch the external test data ===

include (ExternalData)

set (ExternalData_URL_TEMPLATES "http://numerics88.com/data/%(algo)/%(hash)")

ExternalData_Expand_Arguments(
  fetch_data_tarball
  DATATARBALL
  DATA{${CMAKE_SOURCE_DIR}/data/aimio_testdata.tar.gz})
ExternalData_Add_Target (fetch_data_tarball)

# test_bincmp_v2.aim is just a random choice of one of the unpacked data files.
add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/data/test_bincmp_v2.aim
  COMMAND ${CMAKE_COMMAND} -E tar "xvzf" "aimio_testdata.tar.gz"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/data
  DEPENDS fetch_data_tarball)
add_custom_target (test_data
  DEPENDS ${CMAKE_BINARY_DIR}/data/test_bincmp_v2.aim)


# === Actual tests (single google test binary) ===
include_directories (${GTEST_INCLUDE_DIRS})
include_directories (${Boost_INCLUDE_DIRS})

add_executable (AimIOTests
  AimIOTests.cxx)

target_link_libraries (AimIOTests
  AimIO
  ${GTEST_LIBRARIES}
  Boost::filesystem Boost::system)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries (AimIOTests pthread)
endif (CMAKE_SYSTEM_NAME STREQUAL "Linux")

add_test (
  NAME AimIOTests
  COMMAND $<TARGET_FILE:AimIOTests> "${CMAKE_BINARY_DIR}/data")

add_dependencies (AimIOTests test_data)
