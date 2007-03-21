/*
 * -----------------------------------------------------------------
 * $Revision: 1.3 $
 * $Date: 2007-03-21 18:56:33 $
 * ----------------------------------------------------------------- 
 * Programmer: Radu Serban @ LLNL
 * -----------------------------------------------------------------
 * Copyright (c) 2006, The Regents of the University of California.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * -----------------------------------------------------------------
 * Common implementation header file for the CVSDIRECT linear solvers.
 * -----------------------------------------------------------------
 */

#ifndef _CVSDIRECT_IMPL_H
#define _CVSDIRECT_IMPL_H

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

#include <cvodes/cvodes_direct.h>

/*
 * =================================================================
 * C V S D I R E C T    I N T E R N A L    C O N S T A N T S
 * =================================================================
 */

/*
 * -----------------------------------------------------------------
 * CVSDIRECT solver constants
 * -----------------------------------------------------------------
 * CVD_MSBJ   maximum number of steps between Jacobian evaluations
 * CVD_DGMAX  maximum change in gamma between Jacobian evaluations
 * -----------------------------------------------------------------
 */

#define CVD_MSBJ  50
#define CVD_DGMAX RCONST(0.2)

/*
 * =================================================================
 * PART I:  F O R W A R D    P R O B L E M S
 * =================================================================
 */

/*
 * -----------------------------------------------------------------
 * Types: CVDlsMemRec, CVDlsMem                             
 * -----------------------------------------------------------------
 * CVDlsMem is pointer to a CVDlsMemRec structure.
 * -----------------------------------------------------------------
 */

typedef struct {

  int d_type;             /* SUNDIALS_DENSE or SUNDIALS_BAND              */

  int d_n;                /* problem dimension                            */

  int d_ml;               /* lower bandwidth of Jacobian                  */
  int d_mu;               /* upper bandwidth of Jacobian                  */ 
  int d_smu;              /* upper bandwith of M = MIN(N-1,d_mu+d_ml)     */

  CVDlsDenseJacFn d_djac; /* dense Jacobian routine to be called          */
  CVDlsBandJacFn d_bjac;  /* band Jacobian routine to be called           */
  void *d_J_data;         /* user data is passed to d_jac or d_jac        */

  DlsMat d_M;             /* M = I - gamma * df/dy                        */
  DlsMat d_savedJ;        /* savedJ = old Jacobian                        */

  int *d_pivots;          /* pivots = pivot array for PM = LU             */
  
  long int  d_nstlj;      /* nstlj = nst at last Jacobian eval.           */

  long int d_nje;         /* nje = no. of calls to jac                    */

  long int d_nfeDQ;       /* no. of calls to f due to DQ Jacobian approx. */

  int d_last_flag;        /* last error return flag                       */
  
} CVDlsMemRec, *CVDlsMem;

/*
 * -----------------------------------------------------------------
 * Prototypes of internal functions
 * -----------------------------------------------------------------
 */

int cvDlsDenseDQJac(int N, realtype t,
		    N_Vector y, N_Vector fy, 
		    DlsMat Jac, void *jac_data,
		    N_Vector tmp1, N_Vector tmp2, N_Vector tmp3);
  
int cvDlsBandDQJac(int N, int mupper, int mlower,
		   realtype t, N_Vector y, N_Vector fy, 
		   DlsMat Jac, void *jac_data,
		   N_Vector tmp1, N_Vector tmp2, N_Vector tmp3);


/*
 * =================================================================
 * PART II:  B A C K W A R D    P R O B L E M S
 * =================================================================
 */

/*
 * -----------------------------------------------------------------
 * Types : CVDlsMemRecB, CVDlsMemB       
 * -----------------------------------------------------------------
 * A CVSDIRECT linear solver's specification function attaches such
 * a structure to the lmemB filed of CVodeBMem
 * -----------------------------------------------------------------
 */

typedef struct {

  int d_typeB;

  CVDlsDenseJacFnB d_djacB;
  CVDlsBandJacFnB d_bjacB;
  void *d_jac_dataB;

} CVDlsMemRecB, *CVDlsMemB;

/*
 * =================================================================
 * E R R O R   M E S S A G E S
 * =================================================================
 */

#define MSGD_CVMEM_NULL "Integrator memory is NULL."
#define MSGD_BAD_NVECTOR "A required vector operation is not implemented."
#define MSGD_BAD_SIZES "Illegal bandwidth parameter(s). Must have 0 <=  ml, mu <= N-1."
#define MSGD_MEM_FAIL "A memory request failed."
#define MSGD_LMEM_NULL "Linear solver memory is NULL."
#define MSGD_JACFUNC_FAILED "The Jacobian routine failed in an unrecoverable manner."

#define MSGD_CAMEM_NULL "cvb_mem = NULL illegal."
#define MSGD_LMEMB_NULL "Linear solver memory is NULL for the backward integration."
#define MSGD_BAD_T "Bad t for interpolation."

#ifdef __cplusplus
}
#endif

#endif