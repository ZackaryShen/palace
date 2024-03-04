# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

#
# Build Palace
#

# Force build order
set(PALACE_DEPENDENCIES mfem libCEED)
if(PALACE_BUILD_EXTERNAL_DEPS)
  list(APPEND PALACE_DEPENDENCIES json fmt eigen)
  if(PALACE_WITH_SLEPC)
    list(APPEND PALACE_DEPENDENCIES slepc)
  endif()
  if(PALACE_WITH_ARPACK)
    list(APPEND PALACE_DEPENDENCIES arpack-ng)
  endif()
endif()

set(PALACE_OPTIONS ${PALACE_SUPERBUILD_DEFAULT_ARGS})
list(APPEND PALACE_OPTIONS
  "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
  "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
  "-DPALACE_WITH_OPENMP=${PALACE_WITH_OPENMP}"
  "-DPALACE_WITH_SLEPC=${PALACE_WITH_SLEPC}"
  "-DPALACE_WITH_ARPACK=${PALACE_WITH_ARPACK}"
  "-DANALYZE_SOURCES_CLANG_TIDY=${ANALYZE_SOURCES_CLANG_TIDY}"
  "-DANALYZE_SOURCES_CPPCHECK=${ANALYZE_SOURCES_CPPCHECK}"
)
if(PALACE_WITH_ARPACK)
  list(APPEND PALACE_OPTIONS
    "-DCMAKE_Fortran_COMPILER=${CMAKE_Fortran_COMPILER}"
    "-DCMAKE_Fortran_FLAGS=${CMAKE_Fortran_FLAGS}"
  )
endif()

# Configure GPU support
if(PALACE_WITH_CUDA)
  list(APPEND PALACE_OPTIONS
    "-DPALACE_WITH_CUDA=ON"
    "-DCMAKE_CUDA_COMPILER=${CMAKE_CUDA_COMPILER}"
    "-DCMAKE_CUDA_FLAGS=${CMAKE_CUDA_FLAGS}"
  )
  if(NOT "${CMAKE_CUDA_ARCHITECTURES}" STREQUAL "")
    list(APPEND PALACE_OPTIONS
      "-DCMAKE_CUDA_ARCHITECTURES=${CMAKE_CUDA_ARCHITECTURES}"
    )
  endif()
else()
  list(APPEND PALACE_OPTIONS
    "-DPALACE_WITH_CUDA=OFF"
  )
endif()
if(PALACE_WITH_HIP)
  list(APPEND PALACE_OPTIONS
    "-DPALACE_WITH_HIP=ON"
    "-DCMAKE_HIP_COMPILER=${CMAKE_HIP_COMPILER}"
    "-DCMAKE_HIP_FLAGS=${CMAKE_HIP_FLAGS}"
  )
  if(NOT "${CMAKE_HIP_ARCHITECTURES}" STREQUAL "")
    list(APPEND PALACE_OPTIONS
      "-DCMAKE_HIP_ARCHITECTURES=${CMAKE_HIP_ARCHITECTURES}"
    )
  endif()
else()
  list(APPEND PALACE_OPTIONS
    "-DPALACE_WITH_HIP=OFF"
  )
endif()

string(REPLACE ";" "; " PALACE_OPTIONS_PRINT "${PALACE_OPTIONS}")
message(STATUS "PALACE_OPTIONS: ${PALACE_OPTIONS_PRINT}")

include(ExternalProject)
if(POLICY CMP0114)
  cmake_policy(SET CMP0114 NEW)
endif()
ExternalProject_Add(palace
  DEPENDS           ${PALACE_DEPENDENCIES}
  SOURCE_DIR        ${CMAKE_SOURCE_DIR}/palace
  BINARY_DIR        ${CMAKE_BINARY_DIR}/palace-build
  INSTALL_DIR       ${CMAKE_INSTALL_PREFIX}
  PREFIX            ${CMAKE_BINARY_DIR}/palace-cmake
  BUILD_ALWAYS      TRUE
  DOWNLOAD_COMMAND  ""
  CONFIGURE_COMMAND ${CMAKE_COMMAND} <SOURCE_DIR> "${PALACE_OPTIONS}"
  TEST_COMMAND      ""
)

# Add target for Palace unit tests
ExternalProject_Add_Step(palace tests
  COMMAND           ${CMAKE_MAKE_PROGRAM} unit-tests
  DEPENDEES         install
  DEPENDERS         ""
  COMMENT           "Building unit tests for 'palace'"
  WORKING_DIRECTORY <BINARY_DIR>
  EXCLUDE_FROM_MAIN TRUE
)
ExternalProject_Add_StepTargets(palace tests)
