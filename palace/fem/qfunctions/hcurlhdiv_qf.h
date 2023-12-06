// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef PALACE_LIBCEED_HCURL_HDIV_QF_H
#define PALACE_LIBCEED_HCURL_HDIV_QF_H

#include "types_qf.h"
#include "utils_qf.h"

// libCEED QFunctions for mixed H(curl)-H(div) operators (Piola transformations u =
// adj(J)^T / det(J) ̂u and u = J / det(J) ̂u).
// in[0] is Jacobian determinant quadrature data, shape [Q]
// in[1] is transpose adjugate Jacobian quadrature data, shape [ncomp=space_dim*dim, Q]
// in[2] is Jacobian quadrature data, shape [ncomp=space_dim*dim, Q]
// in[3] is active vector, shape [qcomp=dim, ncomp=1, Q]
// in[4] is element attribute, shape [1]
// out[0] is active vector, shape [qcomp=dim, ncomp=1, Q]

CEED_QFUNCTION(f_apply_hcurlhdiv_22)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                                     CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *adjJt = in[1], *J = in[2], *u = in[3];
  CeedScalar *v = out[0];

  const CeedScalar *attr = in[4];
  MatCoeffContext2 *bc = (MatCoeffContext2 *)ctx;
  // const CeedInt attr = (CeedInt)*in[4];
  // const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[attr]];

  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {
    CeedScalar qd[4];

    // XXX TODO TESTING
    const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[(CeedInt)attr[i]]];

    MultAtBC22(J + i, Q, coeff, adjJt + i, Q, qd);

    const CeedScalar u0 = u[i + Q * 0];
    const CeedScalar u1 = u[i + Q * 1];
    v[i + Q * 0] = wdetJ[i] * (qd[0] * u0 + qd[2] * u1);
    v[i + Q * 1] = wdetJ[i] * (qd[1] * u0 + qd[3] * u1);
  }
  return 0;
}

CEED_QFUNCTION(f_apply_hcurlhdiv_33)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                                     CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *adjJt = in[1], *J = in[2], *u = in[3];
  CeedScalar *v = out[0];

  const CeedScalar *attr = in[4];
  MatCoeffContext3 *bc = (MatCoeffContext3 *)ctx;
  // const CeedInt attr = (CeedInt)*in[4];
  // const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[attr]];

  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {
    CeedScalar qd[9];

    // XXX TODO TESTING
    const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[(CeedInt)attr[i]]];

    MultAtBC33(J + i, Q, coeff, adjJt + i, Q, qd);

    const CeedScalar u0 = u[i + Q * 0];
    const CeedScalar u1 = u[i + Q * 1];
    const CeedScalar u2 = u[i + Q * 2];
    v[i + Q * 0] = wdetJ[i] * (qd[0] * u0 + qd[3] * u1 + qd[6] * u2);
    v[i + Q * 1] = wdetJ[i] * (qd[1] * u0 + qd[4] * u1 + qd[7] * u2);
    v[i + Q * 2] = wdetJ[i] * (qd[2] * u0 + qd[5] * u1 + qd[8] * u2);
  }
  return 0;
}

CEED_QFUNCTION(f_apply_hcurlhdiv_21)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                                     CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *adjJt = in[1], *J = in[2], *u = in[3];
  CeedScalar *v = out[0];

  const CeedScalar *attr = in[4];
  MatCoeffContext2 *bc = (MatCoeffContext2 *)ctx;
  // const CeedInt attr = (CeedInt)*in[4];
  // const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[attr]];

  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {
    CeedScalar qd[1];

    // XXX TODO TESTING
    const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[(CeedInt)attr[i]]];

    MultAtBC21(J + i, Q, coeff, adjJt + i, Q, qd);

    const CeedScalar u0 = u[i + Q * 0];
    v[i + Q * 0] = wdetJ[i] * qd[0] * u0;
  }
  return 0;
}

CEED_QFUNCTION(f_apply_hcurlhdiv_32)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                                     CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *adjJt = in[1], *J = in[2], *u = in[3];
  CeedScalar *v = out[0];

  const CeedScalar *attr = in[4];
  MatCoeffContext3 *bc = (MatCoeffContext3 *)ctx;
  // const CeedInt attr = (CeedInt)*in[4];
  // const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[attr]];

  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {
    CeedScalar qd[4];

    // XXX TODO TESTING
    const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[(CeedInt)attr[i]]];

    MultAtBC32(J + i, Q, coeff, adjJt + i, Q, qd);

    const CeedScalar u0 = u[i + Q * 0];
    const CeedScalar u1 = u[i + Q * 1];
    v[i + Q * 0] = wdetJ[i] * (qd[0] * u0 + qd[2] * u1);
    v[i + Q * 1] = wdetJ[i] * (qd[1] * u0 + qd[3] * u1);
  }
  return 0;
}

CEED_QFUNCTION(f_apply_hdivhcurl_22)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                                     CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *adjJt = in[1], *J = in[2], *u = in[3];
  CeedScalar *v = out[0];

  const CeedScalar *attr = in[4];
  MatCoeffContext2 *bc = (MatCoeffContext2 *)ctx;
  // const CeedInt attr = (CeedInt)*in[4];
  // const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[attr]];

  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {
    CeedScalar qd[4];

    // XXX TODO TESTING
    const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[(CeedInt)attr[i]]];

    MultAtBC22(adjJt + i, Q, coeff, J + i, Q, qd);

    const CeedScalar u0 = u[i + Q * 0];
    const CeedScalar u1 = u[i + Q * 1];
    v[i + Q * 0] = wdetJ[i] * (qd[0] * u0 + qd[2] * u1);
    v[i + Q * 1] = wdetJ[i] * (qd[1] * u0 + qd[3] * u1);
  }
  return 0;
}

CEED_QFUNCTION(f_apply_hdivhcurl_33)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                                     CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *adjJt = in[1], *J = in[2], *u = in[3];
  CeedScalar *v = out[0];

  const CeedScalar *attr = in[4];
  MatCoeffContext3 *bc = (MatCoeffContext3 *)ctx;
  // const CeedInt attr = (CeedInt)*in[4];
  // const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[attr]];

  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {
    CeedScalar qd[9];

    // XXX TODO TESTING
    const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[(CeedInt)attr[i]]];

    MultAtBC33(adjJt + i, Q, coeff, J + i, Q, qd);

    const CeedScalar u0 = u[i + Q * 0];
    const CeedScalar u1 = u[i + Q * 1];
    const CeedScalar u2 = u[i + Q * 2];
    v[i + Q * 0] = wdetJ[i] * (qd[0] * u0 + qd[3] * u1 + qd[6] * u2);
    v[i + Q * 1] = wdetJ[i] * (qd[1] * u0 + qd[4] * u1 + qd[7] * u2);
    v[i + Q * 2] = wdetJ[i] * (qd[2] * u0 + qd[5] * u1 + qd[8] * u2);
  }
  return 0;
}

CEED_QFUNCTION(f_apply_hdivhcurl_21)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                                     CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *adjJt = in[1], *J = in[2], *u = in[3];
  CeedScalar *v = out[0];

  const CeedScalar *attr = in[4];
  MatCoeffContext2 *bc = (MatCoeffContext2 *)ctx;
  // const CeedInt attr = (CeedInt)*in[4];
  // const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[attr]];

  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {
    CeedScalar qd[1];

    // XXX TODO TESTING
    const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[(CeedInt)attr[i]]];

    MultAtBC21(adjJt + i, Q, coeff, J + i, Q, qd);

    const CeedScalar u0 = u[i + Q * 0];
    v[i + Q * 0] = wdetJ[i] * qd[0] * u0;
  }
  return 0;
}

CEED_QFUNCTION(f_apply_hdivhcurl_32)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                                     CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *adjJt = in[1], *J = in[2], *u = in[3];
  CeedScalar *v = out[0];

  const CeedScalar *attr = in[4];
  MatCoeffContext3 *bc = (MatCoeffContext3 *)ctx;
  // const CeedInt attr = (CeedInt)*in[4];
  // const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[attr]];

  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {
    CeedScalar qd[4];

    // XXX TODO TESTING
    const CeedScalar *coeff = bc->mat_coeff[bc->attr_mat[(CeedInt)attr[i]]];

    MultAtBC32(adjJt + i, Q, coeff, J + i, Q, qd);

    const CeedScalar u0 = u[i + Q * 0];
    const CeedScalar u1 = u[i + Q * 1];
    v[i + Q * 0] = wdetJ[i] * (qd[0] * u0 + qd[2] * u1);
    v[i + Q * 1] = wdetJ[i] * (qd[1] * u0 + qd[3] * u1);
  }
  return 0;
}

#endif  // PALACE_LIBCEED_HCURL_HDIV_QF_H
