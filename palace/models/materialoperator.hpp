// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef PALACE_MODELS_MATERIAL_OPERATOR_HPP
#define PALACE_MODELS_MATERIAL_OPERATOR_HPP

#include <mfem.hpp>
#include "fem/mesh.hpp"

namespace palace
{

class IoData;

//
// A class handling material attributes.
//
class MaterialOperator
{
private:
  // Reference to underlying mesh object (not owned).
  const Mesh &mesh;

  // Mapping from the local attribute to material index.
  mfem::Array<int> attr_mat;

  // Material properties: relative permeability, relative permittivity, and others (like
  // electrical conductivity and London penetration depth for superconductors.
  mfem::DenseTensor mat_muinv, mat_epsilon, mat_epsilon_imag, mat_epsilon_abs, mat_invz0,
      mat_c0, mat_sigma, mat_invLondon;
  mfem::Array<double> mat_c0_min, mat_c0_max;

  // Domain attributes with nonzero loss tangent, electrical conductivity, London
  // penetration depth.
  mfem::Array<int> losstan_attr, conductivity_attr, london_attr;

  void SetUpMaterialProperties(const IoData &iodata, const mfem::ParMesh &mesh);

  const auto AttrToMat(int attr) const
  {
    const auto &loc_attr = mesh.GetAttributeGlobalToLocal();
    MFEM_ASSERT(loc_attr.find(attr) != loc_attr.end(),
                "Missing local domain attribute for attribute " << attr << "!");
    return attr_mat[loc_attr.at(attr) - 1];
  }

  const auto Wrap(const mfem::DenseTensor &data, int attr) const
  {
    const int k = AttrToMat(attr);
    return mfem::DenseMatrix(const_cast<double *>(data.GetData(k)), data.SizeI(),
                             data.SizeJ());
  }

public:
  MaterialOperator(const IoData &iodata, const Mesh &mesh);

  int SpaceDimension() const { return mat_muinv.SizeI(); }

  const auto GetInvPermeability(int attr) const { return Wrap(mat_muinv, attr); }
  const auto GetPermittivityReal(int attr) const { return Wrap(mat_epsilon, attr); }
  const auto GetPermittivityImag(int attr) const { return Wrap(mat_epsilon_imag, attr); }
  const auto GetPermittivityAbs(int attr) const { return Wrap(mat_epsilon_abs, attr); }
  const auto GetInvImpedance(int attr) const { return Wrap(mat_invz0, attr); }
  const auto GetLightSpeed(int attr) const { return Wrap(mat_c0, attr); }
  const auto GetConductivity(int attr) const { return Wrap(mat_sigma, attr); }
  const auto GetInvLondonDepth(int attr) const { return Wrap(mat_invLondon, attr); }

  auto GetLightSpeedMin(int attr) const { return mat_c0_min[AttrToMat(attr)]; }
  auto GetLightSpeedMax(int attr) const { return mat_c0_max[AttrToMat(attr)]; }

  const auto &GetInvPermeability() const { return mat_muinv; }
  const auto &GetPermittivityReal() const { return mat_epsilon; }
  const auto &GetPermittivityImag() const { return mat_epsilon_imag; }
  const auto &GetPermittivityAbs() const { return mat_epsilon_abs; }
  const auto &GetInvImpedance() const { return mat_invz0; }
  const auto &GetLightSpeed() const { return mat_c0; }
  const auto &GetConductivity() const { return mat_sigma; }
  const auto &GetInvLondonDepth() const { return mat_invLondon; }

  bool HasLossTangent() const { return (losstan_attr.Size() > 0); }
  bool HasConductivity() const { return (conductivity_attr.Size() > 0); }
  bool HasLondonDepth() const { return (london_attr.Size() > 0); }

  const auto &GetAttributeToMaterial() const { return attr_mat; }
  mfem::Array<int> GetBdrAttributeToMaterial() const;

  const auto &GetMesh() const { return mesh; }

  template <typename T>
  auto GetAttributeGlobalToLocal(const T &attr_list) const
  {
    return mesh.GetAttributeGlobalToLocal(attr_list);
  }
  template <typename T>
  auto GetBdrAttributeGlobalToLocal(const T &attr_list) const
  {
    return mesh.GetBdrAttributeGlobalToLocal(attr_list);
  }
};

//
// Material property represented as a piecewise constant coefficient over mesh elements. Can
// be scalar-valued or matrix-valued.
//
class MaterialPropertyCoefficient
{
private:
  // Map attribute to material index (coeff = mat_coeff[attr_mat[attr - 1]], for 1-based
  // attributes).
  mfem::Array<int> attr_mat;

  // Material properry coefficients, ordered by material index.
  mfem::DenseTensor mat_coeff;

public:
  MaterialPropertyCoefficient() {}
  MaterialPropertyCoefficient(const mfem::Array<int> &attr_mat_,
                              const mfem::DenseTensor &mat_coeff_, double a = 1.0);

  bool empty() const { return mat_coeff.TotalSize() == 0; }

  const auto &GetAttributeToMaterial() const { return attr_mat; }
  const auto &GetMaterialProperties() const { return mat_coeff; }

  void AddCoefficient(const mfem::Array<int> &attr_mat_,
                      const mfem::DenseTensor &mat_coeff_, double a = 1.0);

  template <typename T>
  void AddMaterialProperty(const mfem::Array<int> &attr_list, const T &coeff,
                           double a = 1.0);
  template <typename T>
  void AddMaterialProperty(int attr, const T &coeff, double a = 1.0)
  {
    mfem::Array<int> attr_list(1);
    attr_list[0] = attr;
    AddMaterialProperty(attr_list, coeff, a);
  }

  void RestrictCoefficient(const mfem::Array<int> &attr_list);

  void NormalProjectedCoefficient(const mfem::Vector &normal);
};

}  // namespace palace

#endif  // PALACE_MODELS_MATERIAL_OPERATOR_HPP
