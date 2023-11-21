// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef PALACE_LIBCEED_L2_QF_H
#define PALACE_LIBCEED_L2_QF_H

// libCEED QFunctions for L2 operators (Piola transformation u = 1 / det(J) ̂u).
// in[0] is Jacobian determinant quadrature data, shape [Q]
// in[1] is quadrature weights, shape [Q]
// in[2] is active vector, shape [ncomp=vdim, Q]
// out[0] is active vector, shape [ncomp=vdim, Q]

CEED_QFUNCTION(f_apply_l2_1)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                             CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *qw = in[1], *u = in[2];
  CeedScalar *v = out[0];
  const CeedScalar coeff = 1.0;  // XX TODO NON-IDENTITY COEFFICIENTS
  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {
    v[i] = (coeff * qw[i] * qw[i] / wdetJ[i]) * u[i];
  }
  return 0;
}

CEED_QFUNCTION(f_apply_l2_2)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                             CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *qw = in[1], *u = in[2];
  CeedScalar *v = out[0];
  const CeedScalar coeff = 1.0;  // XX TODO NON-IDENTITY COEFFICIENTS
  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {
    const CeedScalar qd = coeff * qw[i] * qw[i] / wdetJ[i];
    CeedPragmaSIMD for (CeedInt d = 0; d < 2; d++)
    {
      v[i + Q * d] = qd * u[i + Q * d];
    }
  }
  return 0;
}

CEED_QFUNCTION(f_apply_l2_3)(void *ctx, CeedInt Q, const CeedScalar *const *in,
                             CeedScalar *const *out)
{
  const CeedScalar *wdetJ = in[0], *qw = in[1], *u = in[2];
  CeedScalar *v = out[0];
  const CeedScalar coeff = 1.0;  // XX TODO NON-IDENTITY COEFFICIENTS
  CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
  {
    const CeedScalar qd = coeff * qw[i] * qw[i] / wdetJ[i];
    CeedPragmaSIMD for (CeedInt d = 0; d < 3; d++)
    {
      v[i + Q * d] = qd * u[i + Q * d];
    }
  }
  return 0;
}

// XX TODO REMOVE AND ADD COEFFICIENT SUPPORT ABOVE

// struct DivDivContext
// {
//   CeedInt dim, space_dim;
//   CeedScalar coeff;
// };

// // libCEED QFunction for building quadrature data for a div-div operator with a constant
// // coefficient.
// CEED_QFUNCTION(f_build_divdiv_const)(void *ctx, CeedInt Q, const CeedScalar *const *in,
//                                      CeedScalar *const *out)
// {
//   // At every quadrature point, compute and store qw * c / det(J).
//   // in[0] is Jacobians with shape [dim, ncomp=space_dim, Q]
//   // in[1] is quadrature weights, size (Q)
//   DivDivContext *bc = (DivDivContext *)ctx;
//   const CeedScalar coeff = bc->coeff;
//   const CeedScalar *J = in[0], *qw = in[1];
//   CeedScalar *qd = out[0];
//   switch (10 * bc->space_dim + bc->dim)
//   {
//     case 11:
//       CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
//       {
//         qd[i] = qw[i] * coeff / J[i];
//       }
//       break;
//     case 21:
//       CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
//       {
//         qd[i] = qw[i] * coeff / DetJ21(J + i, Q);
//       }
//       break;
//     case 22:
//       CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
//       {
//         qd[i] = qw[i] * coeff / DetJ22(J + i, Q);
//       }
//       break;
//     case 32:
//       CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
//       {
//         qd[i] = qw[i] * coeff / DetJ32(J + i, Q);
//       }
//       break;
//     case 33:
//       CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
//       {
//         qd[i] = qw[i] * coeff / DetJ33(J + i, Q);
//       }
//       break;
//   }
//   return 0;
// }

// // libCEED QFunction for building quadrature data for a div-div operator with a
// coefficient
// // evaluated at quadrature points.
// CEED_QFUNCTION(f_build_divdiv_quad)(void *ctx, CeedInt Q, const CeedScalar *const *in,
//                                     CeedScalar *const *out)
// {
//   // At every quadrature point, compute and store qw * c / det(J).
//   // in[0] is coefficients, size (Q)
//   // in[1] is Jacobians with shape [dim, ncomp=space_dim, Q]
//   // in[2] is quadrature weights, size (Q)
//   DivDivContext *bc = (DivDivContext *)ctx;
//   const CeedScalar *c = in[0], *J = in[1], *qw = in[2];
//   CeedScalar *qd = out[0];
//   switch (10 * bc->space_dim + bc->dim)
//   {
//     case 11:
//       CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
//       {
//         qd[i] = qw[i] * c[i] / J[i];
//       }
//       break;
//     case 21:
//       CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
//       {
//         qd[i] = qw[i] * c[i] / DetJ21(J + i, Q);
//       }
//       break;
//     case 22:
//       CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
//       {
//         qd[i] = qw[i] * c[i] / DetJ22(J + i, Q);
//       }
//       break;
//     case 32:
//       CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
//       {
//         qd[i] = qw[i] * c[i] / DetJ32(J + i, Q);
//       }
//       break;
//     case 33:
//       CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
//       {
//         qd[i] = qw[i] * c[i] / DetJ33(J + i, Q);
//       }
//       break;
//   }
//   return 0;
// }

// // libCEED QFunction for applying a div-div operator.
// CEED_QFUNCTION(f_apply_divdiv)(void *ctx, CeedInt Q, const CeedScalar *const *in,
//                                CeedScalar *const *out)
// {
//   // in[0], out[0] have shape [ncomp=1, Q]
//   const CeedScalar *ud = in[0], *qd = in[1];
//   CeedScalar *vd = out[0];
//   CeedPragmaSIMD for (CeedInt i = 0; i < Q; i++)
//   {
//     vd[i] = qd[i] * ud[i];
//   }
//   return 0;
// }

#endif  // PALACE_LIBCEED_L2_QF_H
