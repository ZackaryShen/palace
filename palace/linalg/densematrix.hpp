
// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef PALACE_LINALG_DENSE_MATRIX_HPP
#define PALACE_LINALG_DENSE_MATRIX_HPP

#include <mfem.hpp>

namespace mfem
{

class DenseMatrix;
class DenseTensor;

}  // namespace mfem

namespace palace::linalg
{

//
// Functionality for manipulating small dense matrices which extends the capabilities of
// mfem::DenseMatrix.
//

mfem::DenseMatrix MatrixSqrt(const mfem::DenseMatrix &M);

mfem::DenseTensor MatrixSqrt(const mfem::DenseTensor &T);

mfem::DenseMatrix MatrixPow(const mfem::DenseMatrix &M, mfem::real_t p);

mfem::DenseTensor MatrixPow(const mfem::DenseTensor &T, mfem::real_t p);

mfem::real_t SingularValueMax(const mfem::DenseMatrix &M);

mfem::real_t SingularValueMin(const mfem::DenseMatrix &M);

mfem::DenseTensor Mult(const mfem::DenseTensor &A, const mfem::DenseTensor &B);

}  // namespace palace::linalg

#endif  // PALACE_LINALG_DENSE_MATRIX_HPP
