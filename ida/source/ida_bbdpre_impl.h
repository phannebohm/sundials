/*
 * -----------------------------------------------------------------
 * $Revision: 1.2 $
 * $Date: 2006-01-17 23:30:38 $
 * ----------------------------------------------------------------- 
 * Programmer(s): Alan C. Hindmarsh and Radu Serban @ LLNL
 * -----------------------------------------------------------------
 * Copyright (c) 2002, The Regents of the University of California.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see sundials/ida/LICENSE.
 * -----------------------------------------------------------------
 * This is the header file (private version) for the IDABBDPRE
 * module, for a band-block-diagonal preconditioner, i.e. a
 * block-diagonal matrix with banded blocks, for use with IDA/IDAS
 * and IDASp*.
 * -----------------------------------------------------------------
 */

#ifndef _IDABBDPRE_IMPL_H
#define _IDABBDPRE_IMPL_H

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

#include "ida_bbdpre.h"
#include "sundials_band.h"
#include "ida_impl.h"

/*
 * -----------------------------------------------------------------
 * Definition of IBBDPrecData
 * -----------------------------------------------------------------
 */

typedef struct {

  /* passed by user to IDABBDPrecAlloc and used by
     IDABBDPrecSetup/IDABBDPrecSolve functions */

  long int mudq, mldq, mukeep, mlkeep;
  realtype rel_yy;
  IDABBDLocalFn glocal;
  IDABBDCommFn gcomm;

 /* allocated for use by IDABBDPrecSetup */

  N_Vector tempv4;

  /* set by IDABBDPrecon and used by IDABBDPrecSolve */

  BandMat PP;
  long int *pivots;

  /* set by IDABBDPrecAlloc and used by IDABBDPrecSetup */

  long int n_local;

  /* available for optional output */

  long int rpwsize;
  long int ipwsize;
  long int nge;

  /* pointer to ida_mem */

  IDAMem IDA_mem;

} *IBBDPrecData;

/*
 * -----------------------------------------------------------------
 * IDABBDPRE error messages
 * -----------------------------------------------------------------
 */

#define MSGBBD_IDAMEM_NULL "IDABBDPrecAlloc-- integrator memory is NULL.\n\n"
#define MSGBBD_BAD_NVECTOR "IDABBDPrecAlloc-- a required vector operation is not implemented.\n\n"
#define MSGBBD_WRONG_NVEC  "IDABBDPrecAlloc-- incompatible NVECTOR implementation.\n\n"

#define MSGBBD_PDATA_NULL "IDABBDPrecReInit/IDABBDPrecGet*-- IBBDPrecData is NULL.\n\n"

#define MSGBBD_NO_PDATA "IDABBDSp*-- IBBDPrecData is NULL.\n\n"

#ifdef __cplusplus
}
#endif

#endif
