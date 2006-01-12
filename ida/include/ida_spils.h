/*
 * -----------------------------------------------------------------
 * $Revision: 1.1 $
 * $Date: 2006-01-11 21:13:53 $
 * ----------------------------------------------------------------- 
 * Programmers: Alan Hindmarsh, Radu Serban and Aaron Collier @ LLNL
 * -----------------------------------------------------------------
 * Copyright (c) 2002, The Regents of the University of California  
 * Produced at the Lawrence Livermore National Laboratory
 * All rights reserved
 * For details, see sundials/ida/LICENSE
 * -----------------------------------------------------------------
 * This is the common header file for the Scaled and Preconditioned
 * Iterative Linear Solvers in IDA/IDAS.
 * -----------------------------------------------------------------
 */

#ifndef _IDASPILS_H
#define _IDASPILS_H

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

#include "sundials_nvector.h"

/*
 * -----------------------------------------------------------------
 * Type : IDASpilsPrecSetupFn
 * -----------------------------------------------------------------
 * The optional user-supplied functions PrecSetup and PrecSolve
 * together must define the left preconditoner matrix P
 * approximating the system Jacobian matrix
 *    J = dF/dy + c_j*dF/dy'
 * (where the DAE system is F(t,y,y') = 0), and solve the linear
 * systems P z = r.   PrecSetup is to do any necessary setup
 * operations, and PrecSolve is to compute the solution of
 * P z = r.
 *
 * The preconditioner setup function PrecSetup is to evaluate and
 * preprocess any Jacobian-related data needed by the
 * preconditioner solve function PrecSolve.  This might include
 * forming a crude approximate Jacobian, and performing an LU
 * factorization on it.  This function will not be called in
 * advance of every call to PrecSolve, but instead will be called
 * only as often as necessary to achieve convergence within the
 * Newton iteration.  If the PrecSolve function needs no
 * preparation, the PrecSetup function can be NULL.
 *
 * Each call to the PrecSetup function is preceded by a call to
 * the system function res with the same (t,y,y') arguments.
 * Thus the PrecSetup function can use any auxiliary data that is
 * computed and saved by the res function and made accessible
 * to PrecSetup.
 *
 * A preconditioner setup function PrecSetup must have the
 * prototype given below.  Its parameters are as follows:
 *
 * tt  is the current value of the independent variable t.
 *
 * yy  is the current value of the dependent variable vector,
 *     namely the predicted value of y(t).
 *
 * yp  is the current value of the derivative vector y',
 *     namely the predicted value of y'(t).
 *
 * rr  is the current value of the residual vector F(t,y,y').
 *
 * c_j is the scalar in the system Jacobian, proportional to 1/hh.
 *
 * prec_data is a pointer to user preconditioner data - the same as
 *     the pdata parameter passed to IDASp*.
 *
 * tmp1, tmp2, tmp3 are pointers to vectors of type N_Vector
 *     which can be used by an IDASpilsPrecSetupFn routine
 *     as temporary storage or work space.
 *
 * NOTE: If the user's preconditioner needs other quantities,
 *     they are accessible as follows: hcur (the current stepsize)
 *     and ewt (the error weight vector) are accessible through
 *     IDAGetCurrentStep and IDAGetErrWeights, respectively (see
 *     ida.h). The unit roundoff is available as
 *     UNIT_ROUNDOFF defined in sundialstypes.h
 *
 * The IDASpilsPrecSetupFn should return
 *     0 if successful,
 *     a positive int if a recoverable error occurred, or
 *     a negative int if a nonrecoverable error occurred.
 * In the case of a recoverable error return, the integrator will
 * attempt to recover by reducing the stepsize (which changes cj).
 * -----------------------------------------------------------------
 */

typedef int (*IDASpilsPrecSetupFn)(realtype tt,
                                   N_Vector yy, N_Vector yp, N_Vector rr,
                                   realtype c_j, void *prec_data,
                                   N_Vector tmp1, N_Vector tmp2,
                                   N_Vector tmp3);

/*
 * -----------------------------------------------------------------
 * Type : IDASpilsPrecSolveFn
 * -----------------------------------------------------------------
 * The optional user-supplied function PrecSolve must compute a
 * solution to the linear system P z = r, where P is the left
 * preconditioner defined by the user.  If no preconditioning
 * is desired, pass NULL for PrecSolve to IDASp*.
 *
 * A preconditioner solve function PrecSolve must have the
 * prototype given below.  Its parameters are as follows:
 *
 * tt is the current value of the independent variable t.
 *
 * yy is the current value of the dependent variable vector y.
 *
 * yp is the current value of the derivative vector y'.
 *
 * rr is the current value of the residual vector F(t,y,y').
 *
 * rvec is the input right-hand side vector r.
 *
 * zvec is the computed solution vector z.
 *
 * c_j is the scalar in the system Jacobian, proportional to 1/hh.
 *
 * delta is an input tolerance for use by PrecSolve if it uses an
 *     iterative method in its solution.   In that case, the
 *     the residual vector r - P z of the system should be
 *     made less than delta in weighted L2 norm, i.e.,
 *            sqrt [ Sum (Res[i]*ewt[i])^2 ] < delta .
 *     Note: the error weight vector ewt can be obtained
 *     through a call to the routine IDAGetErrWeights.
 *
 * prec_data is a pointer to user preconditioner data - the same as
 *     the pdata parameter passed to IDASp*.
 *
 * tmp is an N_Vector which can be used by the PrecSolve
 *     routine as temporary storage or work space.
 *
 * The IDASpilsPrecSolveFn should return
 *     0 if successful,
 *     a positive int if a recoverable error occurred, or
 *     a negative int if a nonrecoverable error occurred.
 * Following a recoverable error, the integrator will attempt to
 * recover by updating the preconditioner and/or reducing the
 * stepsize.
 * -----------------------------------------------------------------
 */

typedef int (*IDASpilsPrecSolveFn)(realtype tt,
                                   N_Vector yy, N_Vector yp, N_Vector rr,
                                   N_Vector rvec, N_Vector zvec,
                                   realtype c_j, realtype delta, void *prec_data,
                                   N_Vector tmp);

/*
 * -----------------------------------------------------------------
 * Type : IDASpilsJacTimesVecFn
 * -----------------------------------------------------------------
 * The user-supplied function jtimes is to generate the product
 * J*v for given v, where J is the Jacobian matrix
 *    J = dF/dy + c_j*dF/dy'
 *  or an approximation to it, and v is a given vector.
 * It should return 0 if successful and a nonzero int otherwise.
 *
 * A function jtimes must have the prototype given below. Its
 * parameters are as follows:
 *
 *   tt   is the current value of the independent variable.
 *
 *   yy   is the current value of the dependent variable vector,
 *        namely the predicted value of y(t).
 *
 *   yp   is the current value of the derivative vector y',
 *        namely the predicted value of y'(t).
 *
 *   rr   is the current value of the residual vector F(t,y,y').
 *
 *   v    is the N_Vector to be multiplied by J.
 *
 *   Jv   is the output N_Vector containing J*v.
 *
 *   c_j  is the scalar in the system Jacobian, proportional
 *        to 1/hh.
 *
 *   jac_data is a pointer to user Jacobian data, the same as the
 *        pointer passed to IDASp*.
 *
 *   tmp1, tmp2 are two N_Vectors which can be used by Jtimes for
 *         work space.
 * -----------------------------------------------------------------
 */

typedef int (*IDASpilsJacTimesVecFn)(realtype tt,
                                     N_Vector yy, N_Vector yp, N_Vector rr,
                                     N_Vector v, N_Vector Jv,
                                     realtype c_j, void *jac_data,
                                     N_Vector tmp1, N_Vector tmp2);

#ifdef __cplusplus
}
#endif

#endif