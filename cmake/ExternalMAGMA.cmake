# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

#
# Build MAGMA
#

# Force build order
set(MAGMA_DEPENDENCIES)

set(MAGMA_OPTIONS ${PALACE_SUPERBUILD_DEFAULT_ARGS})
list(APPEND MAGMA_OPTIONS
  "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
  "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
  "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
  "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
  "-DUSE_FORTRAN=OFF"
)

# Always build MAGMA as a shared library
list(TRANSFORM MAGMA_OPTIONS REPLACE
 ".*BUILD_SHARED_LIBS.*" "-DBUILD_SHARED_LIBS=ON"
)

# Configure LAPACK dependency
if(NOT "${BLAS_LAPACK_LIBRARIES}" STREQUAL "")
  list(APPEND MAGMA_OPTIONS
    "-DLAPACK_LIBRARIES=${BLAS_LAPACK_LIBRARIES}"
  )
endif()

# Configure GPU support
if(PALACE_WITH_CUDA)
  list(APPEND MAGMA_OPTIONS
    "-DMAGMA_ENABLE_CUDA=ON"
    "-DCMAKE_CUDA_COMPILER=${CMAKE_CUDA_COMPILER}"
    "-DCUDA_NVCC_FLAGS=${CMAKE_CUDA_FLAGS}"
  )
  set(MAKE_GENERATE_INC "BACKEND = cuda\\n")
  if(NOT "${CMAKE_CUDA_ARCHITECTURES}" STREQUAL "")
    set(MAGMA_CUDA_ARCH)
    foreach(ARCH IN LISTS CMAKE_CUDA_ARCHITECTURES)
      list(APPEND MAGMA_CUDA_ARCH sm_${ARCH})
    endforeach()
    string(REPLACE ";" " " MAGMA_CUDA_ARCH "${MAGMA_CUDA_ARCH}")
    list(APPEND MAGMA_OPTIONS
      "-DGPU_TARGET=${MAGMA_CUDA_ARCH}"
      "-DCMAKE_CUDA_ARCHITECTURES=${CMAKE_CUDA_ARCHITECTURES}"
    )
    set(MAKE_GENERATE_INC "${MAKE_GENERATE_INC}GPU_TARGET = ${MAGMA_CUDA_ARCH}\\n")
  endif()
endif()
if(PALACE_WITH_HIP)
  list(APPEND MAGMA_OPTIONS
    "-DMAGMA_ENABLE_HIP=ON"
    "-DCMAKE_HIP_COMPILER=${CMAKE_HIP_COMPILER}"
    "-DCUDA_HIP_FLAGS=${CMAKE_HIP_FLAGS}"
  )
  set(MAKE_GENERATE_INC "BACKEND = hip\\n")
  if(NOT "${CMAKE_HIP_ARCHITECTURES}" STREQUAL "")
    set(MAGMA_HIP_ARCH)
    foreach(ARCH IN LISTS CMAKE_HIP_ARCHITECTURES)
      list(APPEND MAGMA_HIP_ARCH sm_${ARCH})
    endforeach()
    string(REPLACE ";" " " MAGMA_HIP_ARCH "${MAGMA_HIP_ARCH}")
    list(APPEND MAGMA_OPTIONS
      "-DGPU_TARGET=${MAGMA_HIP_ARCH}"
      "-DCMAKE_HIP_ARCHITECTURES=${CMAKE_HIP_ARCHITECTURES}"
    )
    set(MAKE_GENERATE_INC "${MAKE_GENERATE_INC}GPU_TARGET = ${MAGMA_HIP_ARCH}\\n")
  endif()
endif()
set(MAKE_GENERATE_INC "${MAKE_GENERATE_INC}FORT = true\\n")
file(WRITE ${CMAKE_BINARY_DIR}/extern/magma-cmake/make.inc.cmake
  "file(WRITE make.inc \"${MAKE_GENERATE_INC}\")\n"
)

string(REPLACE ";" "; " MAGMA_OPTIONS_PRINT "${MAGMA_OPTIONS}")
message(STATUS "MAGMA_OPTIONS: ${MAGMA_OPTIONS_PRINT}")

include(ExternalProject)
ExternalProject_Add(magma
  DEPENDS           ${MAGMA_DEPENDENCIES}
  GIT_REPOSITORY    ${EXTERN_MAGMA_URL}
  GIT_TAG           ${EXTERN_MAGMA_GIT_TAG}
  SOURCE_DIR        ${CMAKE_BINARY_DIR}/extern/magma
  BINARY_DIR        ${CMAKE_BINARY_DIR}/extern/magma-build
  INSTALL_DIR       ${CMAKE_INSTALL_PREFIX}
  PREFIX            ${CMAKE_BINARY_DIR}/extern/magma-cmake
  UPDATE_COMMAND    ""
  PATCH_COMMAND
    ${CMAKE_COMMAND} -P ../magma-cmake/make.inc.cmake &&
    ${CMAKE_MAKE_PROGRAM} generate
  CONFIGURE_COMMAND ${CMAKE_COMMAND} <SOURCE_DIR> "${MAGMA_OPTIONS}"
  TEST_COMMAND      ""
)
