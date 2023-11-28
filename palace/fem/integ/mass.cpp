// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "fem/integrator.hpp"

#include "fem/libceed/integrator.hpp"
#include "fem/libceed/utils.hpp"

#include "fem/qfunctions/h1_qf.h"

namespace palace
{

namespace
{

struct MassIntegratorInfo : public ceed::IntegratorInfo
{
  bool ctx;  // XX TODO WIP COEFFICIENTS
};

}  // namespace

void MassIntegrator::Assemble(const ceed::CeedGeomFactorData &geom_data, Ceed ceed,
                              CeedElemRestriction trial_restr,
                              CeedElemRestriction test_restr, CeedBasis trial_basis,
                              CeedBasis test_basis, CeedOperator *op)
{
  MassIntegratorInfo info;

  // Set up geometry factor quadrature data.
  MFEM_VERIFY(geom_data->wdetJ_vec && geom_data->wdetJ_restr,
              "Missing geometry factor quadrature data for MassIntegrator!");
  info.geom_info = ceed::GeomFactorInfo::Determinant;

  // Set up QFunctions.
  CeedInt trial_ncomp, test_ncomp;
  PalaceCeedCall(ceed, CeedBasisGetNumComponents(trial_basis, &trial_ncomp));
  PalaceCeedCall(ceed, CeedBasisGetNumComponents(test_basis, &test_ncomp));
  MFEM_VERIFY(
      trial_ncomp == test_ncomp,
      "MassIntegrator requires test and trial spaces with same number of components!");
  switch (trial_ncomp)
  {
    case 1:
      info.apply_qf = f_apply_h1_1;
      info.apply_qf_path = PalaceQFunctionRelativePath(f_apply_h1_1_loc);
      break;
    case 2:
      info.apply_qf = f_apply_h1_2;
      info.apply_qf_path = PalaceQFunctionRelativePath(f_apply_h1_2_loc);
      break;
    case 3:
      info.apply_qf = f_apply_h1_3;
      info.apply_qf_path = PalaceQFunctionRelativePath(f_apply_h1_3_loc);
      break;
    default:
      MFEM_ABORT("Invalid value of ncomp = " << trial_ncomp << " for MassIntegrator!");
  }
  info.trial_ops = ceed::EvalMode::Interp;
  info.test_ops = ceed::EvalMode::Interp;

  ceed::AssembleCeedOperator(info, geom_data, ceed, trial_restr, test_restr, trial_basis,
                             test_basis, op);
}

}  // namespace palace
