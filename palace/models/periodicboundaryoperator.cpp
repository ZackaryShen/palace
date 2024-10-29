// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "periodicboundaryoperator.hpp"

#include <set>
#include "linalg/densematrix.hpp"
#include "models/materialoperator.hpp"
#include "utils/communication.hpp"
#include "utils/geodata.hpp"
#include "utils/iodata.hpp"
#include "utils/prettyprint.hpp"

namespace palace
{

PeriodicBoundaryOperator::PeriodicBoundaryOperator(const IoData &iodata,
                                                   const MaterialOperator &mat_op,
                                                   const mfem::ParMesh &mesh)
  : mat_op(mat_op), periodic_attr(SetUpBoundaryProperties(iodata, mesh))
{
  // Print out BC info for all periodic attributes.
  if (periodic_attr.Size())
  {
    Mpi::Print("\nConfiguring periodic BC at attributes:\n");
    std::sort(periodic_attr.begin(), periodic_attr.end());
    utils::PrettyPrint(periodic_attr);
  }
}

mfem::Array<int>
PeriodicBoundaryOperator::SetUpBoundaryProperties(const IoData &iodata,
                                                  const mfem::ParMesh &mesh)
{
  // Check that periodic boundary attributes have been specified correctly.
  int bdr_attr_max = mesh.bdr_attributes.Size() ? mesh.bdr_attributes.Max() : 0;
  mfem::Array<int> bdr_attr_marker;
  if (!iodata.boundaries.periodic.empty())
  {
    bdr_attr_marker.SetSize(bdr_attr_max);
    bdr_attr_marker = 0;
    for (auto attr : mesh.bdr_attributes)
    {
      bdr_attr_marker[attr - 1] = 1;
    }
    std::set<int> bdr_warn_list;
    for (const auto &data : iodata.boundaries.periodic)
    {
      const auto &da = data.donor_attributes, &ra = data.receiver_attributes;
      for (const auto attr : da)
      {
        if (attr <= 0 || attr > bdr_attr_max || !bdr_attr_marker[attr - 1])
        {
          bdr_warn_list.insert(attr);
        }
      }
      for (const auto attr : ra)
      {
        if (attr <= 0 || attr > bdr_attr_max || !bdr_attr_marker[attr - 1])
        {
          bdr_warn_list.insert(attr);
        }
      }
    }
    if (!bdr_warn_list.empty())
    {
      Mpi::Print("\n");
      Mpi::Warning(
          "Unknown periodic boundary attributes!\nSolver will just ignore them!");
      utils::PrettyPrint(bdr_warn_list, "Boundary attribute list:");
      Mpi::Print("\n");
    }
  }

  // Mark selected boundary attributes from the mesh as periodic.
  // ???? IS THIS USEFUL???
  mfem::Array<int> periodic_bcs;
  for (const auto &data : iodata.boundaries.periodic)
  {
    const auto &da = data.donor_attributes, &ra = data.receiver_attributes;
    for (const auto attr : da)
    {
      if (attr <= 0 || attr > bdr_attr_max || !bdr_attr_marker[attr - 1])
      {
        continue;  // Can just ignore if wrong
      }
      periodic_bcs.Append(attr);
    }
    for (const auto attr : ra)
    {
      if (attr <= 0 || attr > bdr_attr_max || !bdr_attr_marker[attr - 1])
      {
        continue;  // Can just ignore if wrong
      }
      periodic_bcs.Append(attr);
    }

    // Wave vector ???? SHOULD BE ONLY ONE WAVE VECTOR FOR THE ENTIRE SIM
    // NOT ONE PER PERIODIC BC PAIR??? MOVE THIS OUTSIDE THE LOOP?
    MFEM_VERIFY(data.wave_vector.size() == mesh.SpaceDimension(),
    "Block wave vector size must equal the spatial dimension.");
    wave_vector.SetSize(data.wave_vector.size());
    std::copy(data.wave_vector.begin(), data.wave_vector.end(), wave_vector.GetData());
    MFEM_VERIFY(periodic_bcs.Size() == 0 ||
                wave_vector.Normlinf() < std::numeric_limits<double>::epsilon() ||
                iodata.problem.type == config::ProblemData::Type::DRIVEN ||
                iodata.problem.type == config::ProblemData::Type::EIGENMODE,
                "Quasi-periodic Floquet boundary conditions are only available for "
                " frequency domain driven or eigenmode simulations!");


    // Matrix representation of cross product with wave vector
    // [k x] = | 0  -k3  k2|
    //         | k3  0  -k1|
    //         |-k2  k1  0 |
    wave_vector_cross.SetSize(3); // assumes 3D?
    wave_vector_cross(0,1) = -wave_vector[2];
    wave_vector_cross(0,2) = wave_vector[1];
    wave_vector_cross(1,0) = wave_vector[2];
    wave_vector_cross(1,2) = -wave_vector[0];
    wave_vector_cross(2,0) = -wave_vector[1];
    wave_vector_cross(2,1) = wave_vector[0];
  }

  return periodic_bcs;
}

void PeriodicBoundaryOperator::AddRealMassCoefficients(double coeff,
                                                       MaterialPropertyCoefficient &f)
{

  if (periodic_attr.Size())
  {
    // [k x]^T 1/mu [k x]
    mfem::DenseTensor kx(mat_op.GetInvPermeability().SizeI(),
                         mat_op.GetInvPermeability().SizeJ(),
                         mat_op.GetInvPermeability().SizeK());
    mfem::DenseTensor kxT(kx.SizeI(), kx.SizeJ(), kx.SizeK());
    for (int k = 0; k < kx.SizeK(); k++)
    {
      kx(k)  = wave_vector_cross;
      kxT(k).Transpose(wave_vector_cross);
    }
    mfem::DenseTensor kxTmuinvkx = linalg::Mult(mat_op.GetInvPermeability(), kx);
    kxTmuinvkx = linalg::Mult(kxT, kxTmuinvkx);
    MaterialPropertyCoefficient kxTmuinvkx_func(mat_op.GetAttributeToMaterial(), kxTmuinvkx);
    //muinvkx_func.RestrictCoefficient
    f.AddCoefficient(kxTmuinvkx_func.GetAttributeToMaterial(),
                     kxTmuinvkx_func.GetMaterialProperties(), coeff);
  }
}

void PeriodicBoundaryOperator::AddWeakCurlCoefficients(double coeff,
                                                       MaterialPropertyCoefficient &f)
{

  if (periodic_attr.Size())
  {
    // 1/mu [k x]
    mfem::DenseTensor kx(mat_op.GetInvPermeability().SizeI(),
                         mat_op.GetInvPermeability().SizeJ(),
                         mat_op.GetInvPermeability().SizeK());
    for (int k = 0; k < kx.SizeK(); k++)
    {
      kx(k)  = wave_vector_cross;
    }
    mfem::DenseTensor muinvkx = linalg::Mult(mat_op.GetInvPermeability(), kx);
    MaterialPropertyCoefficient muinvkx_func(mat_op.GetAttributeToMaterial(), muinvkx);
    //muinvkx_func.RestrictCoefficient
    f.AddCoefficient(muinvkx_func.GetAttributeToMaterial(),
                     muinvkx_func.GetMaterialProperties(), coeff);
  }
}

void PeriodicBoundaryOperator::AddCurlCoefficients(double coeff,
                                                   MaterialPropertyCoefficient &f)
{

  if (periodic_attr.Size())
  {
    // [k x]^T 1/mu
    mfem::DenseTensor kxT(mat_op.GetInvPermeability().SizeI(),
                          mat_op.GetInvPermeability().SizeJ(),
                          mat_op.GetInvPermeability().SizeK());
    for (int k = 0; k < kxT.SizeK(); k++)
    {
      kxT(k).Transpose(wave_vector_cross);
    }
    mfem::DenseTensor kxTmuinv = linalg::Mult(kxT, mat_op.GetInvPermeability());
    MaterialPropertyCoefficient kxTmuinv_func(mat_op.GetAttributeToMaterial(), kxTmuinv);
    //muinvkx_func.RestrictCoefficient
    f.AddCoefficient(kxTmuinv_func.GetAttributeToMaterial(),
                     kxTmuinv_func.GetMaterialProperties(), coeff);
  }
}
}  // namespace palace
