// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef PALACE_LIBCEED_L2_QF_H
#define PALACE_LIBCEED_L2_QF_H

#include "types_qf.h"

// libCEED QFunctions for L2 operators (Piola transformation u = 1 / det(J) ̂u).
// in[0] is Jacobian determinant quadrature data, shape [Q]
// in[1] is quadrature weights, shape [Q]
// in[2] is active vector, shape [ncomp=vdim, Q]
// in[3] is element attribute, shape [1]
// out[0] is active vector, shape [ncomp=vdim, Q]

CEED_QFUNCTION(f_apply_l2_1)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                             CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *qw = in[1], *u = in[2];
  CeedScalar *v = out[0];

  const CeedScalar *attr = in[3];
  MatCoeffContext1 *bc = (MatCoeffContext1 *)ctx;
  // const CeedInt attr = (CeedInt)*in[3];
  // const CeedScalar coeff = *bc->mat_coeff[bc->attr_mat[attr]];

  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {

    // XXX TODO TESTING
    const CeedScalar coeff = *bc->mat_coeff[bc->attr_mat[(CeedInt)attr[i]]];

    v[i] = (coeff * qw[i] * qw[i] / wdetJ[i]) * u[i];
  }
  return 0;
}

CEED_QFUNCTION(f_apply_l2_2)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                             CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *qw = in[1], *u = in[2];
  CeedScalar *v = out[0];

  const CeedScalar *attr = in[3];
  MatCoeffContext2 *bc = (MatCoeffContext2 *)ctx;
  // const CeedInt attr = (CeedInt)*in[3];
  // const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[attr]];

  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {

    // XXX TODO TESTING
    const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[(CeedInt)attr[i]]];

    const CeedScalar qd = qw[i] * qw[i] / wdetJ[i];

    const CeedScalar u0 = u[i + Q * 0];
    const CeedScalar u1 = u[i + Q * 1];
    v[i + Q * 0] = qd * (coeff[0] * u0 + coeff[1] * u1);
    v[i + Q * 1] = qd * (coeff[1] * u0 + coeff[2] * u1);
  }
  return 0;
}

CEED_QFUNCTION(f_apply_l2_3)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                             CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *qw = in[1], *u = in[2];
  CeedScalar *v = out[0];

  const CeedScalar *attr = in[3];
  MatCoeffContext3 *bc = (MatCoeffContext3 *)ctx;
  // const CeedInt attr = (CeedInt)*in[3];
  // const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[attr]];

  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {

    // XXX TODO TESTING
    const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[(CeedInt)attr[i]]];

    const CeedScalar qd = qw[i] * qw[i] / wdetJ[i];

    const CeedScalar u0 = u[i + Q * 0];
    const CeedScalar u1 = u[i + Q * 1];
    const CeedScalar u2 = u[i + Q * 2];
    v[i + Q * 0] = qd * (coeff[0] * u0 + coeff[1] * u1 + coeff[2] * u2);
    v[i + Q * 1] = qd * (coeff[1] * u0 + coeff[3] * u1 + coeff[4] * u2);
    v[i + Q * 2] = qd * (coeff[2] * u0 + coeff[4] * u1 + coeff[5] * u2);
  }
  return 0;
}

#endif  // PALACE_LIBCEED_L2_QF_H