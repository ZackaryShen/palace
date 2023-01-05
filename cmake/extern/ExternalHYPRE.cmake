# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

#
# Build HYPRE
#

# Force build order
if(PALACE_WITH_ARPACK)
  set(HYPRE_DEPENDENCIES arpack-ng)
elseif(PALACE_WITH_MUMPS)
  set(HYPRE_DEPENDENCIES mumps)
elseif(PALACE_WITH_STRUMPACK)
  set(HYPRE_DEPENDENCIES strumpack)
elseif(PALACE_WITH_SUPERLU)
  set(HYPRE_DEPENDENCIES superlu_dist)
else()
  set(HYPRE_DEPENDENCIES parmetis)
endif()

set(HYPRE_OPTIONS ${PALACE_SUPERBUILD_DEFAULT_ARGS})
list(APPEND HYPRE_OPTIONS
  "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
  "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
  "-DHYPRE_ENABLE_SHARED=${BUILD_SHARED_LIBS}"
  "-DHYPRE_ENABLE_MIXEDINT=${PALACE_WITH_64BIT_INT}"
  "-DHYPRE_ENABLE_BIGINT=OFF"
  "-DHYPRE_WITH_MPI=ON"
  "-DHYPRE_WITH_OPENMP=${PALACE_WITH_OPENMP}"
)

# Configure BLAS/LAPACK
if(NOT "${BLAS_LAPACK_LIBRARIES}" STREQUAL "")
  list(APPEND HYPRE_OPTIONS
    "-DHYPRE_ENABLE_HYPRE_BLAS=OFF"
    "-DHYPRE_ENABLE_HYPRE_LAPACK=OFF"
    "-DLAPACK_LIBRARIES=${BLAS_LAPACK_LIBRARIES}"
    "-DBLAS_LIBRARIES=${BLAS_LAPACK_LIBRARIES}"
  )
endif()

string(REPLACE ";" "; " HYPRE_OPTIONS_PRINT "${HYPRE_OPTIONS}")
message(STATUS "HYPRE_OPTIONS: ${HYPRE_OPTIONS_PRINT}")

include(ExternalProject)
ExternalProject_Add(hypre
  DEPENDS           ${HYPRE_DEPENDENCIES}
  GIT_REPOSITORY    ${CMAKE_CURRENT_SOURCE_DIR}/hypre
  GIT_TAG           ${EXTERN_HYPRE_GIT_TAG}
  SOURCE_DIR        ${CMAKE_CURRENT_BINARY_DIR}/hypre
  BINARY_DIR        ${CMAKE_CURRENT_BINARY_DIR}/hypre-build
  INSTALL_DIR       ${CMAKE_INSTALL_PREFIX}
  PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/hypre-cmake
  UPDATE_COMMAND    ""
  CONFIGURE_COMMAND cmake <SOURCE_DIR>/src "${HYPRE_OPTIONS}"
  TEST_COMMAND      ""
)
