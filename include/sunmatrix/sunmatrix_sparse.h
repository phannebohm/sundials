/*
 * -----------------------------------------------------------------
 * Programmer(s): Daniel Reynolds @ SMU
 *                David Gardner, Carol Woodward, Slaven Peles @ LLNL
 * -----------------------------------------------------------------
 * LLNS/SMU Copyright Start
 * Copyright (c) 2017, Southern Methodist University and 
 * Lawrence Livermore National Security
 *
 * This work was performed under the auspices of the U.S. Department 
 * of Energy by Southern Methodist University and Lawrence Livermore 
 * National Laboratory under Contract DE-AC52-07NA27344.
 * Produced at Southern Methodist University and the Lawrence 
 * Livermore National Laboratory.
 *
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS/SMU Copyright End
 * -----------------------------------------------------------------
 * This is the header file for the sparse implementation of the 
 * SUNMATRIX module.
 * 
 * Part I contains declarations specific to the sparse implementation
 * of the supplied SUNMATRIX module.
 * 
 * Part II defines accessor macros that allow the user to 
 * efficiently use this SUNMatrix type without making explicit
 * references to the underlying data structure.
 *
 * Part III contains the prototype for the constructor 
 * SUNMatrixNew_Sparse as well as implementation-specific prototypes 
 * for various useful matrix operations.
 *
 * Notes:
 *
 *   - The definition of the generic SUNMatrix structure can be found
 *     in the header file sundials_matrix.h.
 *
 *   - The definition of the type 'realtype' can be found in the
 *     header file sundials_types.h, and it may be changed (at the 
 *     configuration stage) according to the user's needs. 
 *     The sundials_types.h file also contains the definition
 *     for the type 'booleantype' and 'indextype'.
 *
 * -----------------------------------------------------------------
 */

#ifndef _SUNMATRIX_SPARSE_H
#define _SUNMATRIX_SPARSE_H

#include <sundials/sundials_matrix.h>
#include <sunmatrix/sunmatrix_dense.h>
#include <sunmatrix/sunmatrix_band.h>
#include <nvector/nvector_serial.h>
#include <nvector/nvector_openmp.h>
#include <nvector/nvector_pthreads.h>

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

/*
 * ==================================================================
 * Type definitions
 * ==================================================================
 */

#define CSC_MAT 0
#define CSR_MAT 1

  
/*
 * -----------------------------------------------------------------
 * PART I: Sparse implementation of SUNMatrix
 *
 * The sparse implementation of the SUNMatrix 'content' structure
 * contains:
 *   M     - number of rows
 *   N     - number of columns
 *   NNZ   - the number of nonzero entries in the matrix
 *   NP    - number of index pointers
 *   data  - pointer to a contiguous block of realtype variables
 *   sparsetype - type of sparse matrix: compressed sparse column or row
 *   indexvals  - indices of each nonzero entry (columns or rows)
 *   indexptrs  - starting index of the first entry in data for each slice
 *   rowvals - pointer to row indices of each nonzero entry
 *   colptrs - pointer to starting indices in data array for each column
 *   colvals - pointer to column indices of each nonzero entry
 *   rowptrs - pointer to starting indices in data array for each row
 * -----------------------------------------------------------------
 */
  
struct _SUNMatrixContent_Sparse {
  long int M;
  long int N;
  long int NNZ;
  long int NP;
  realtype *data;
  int sparsetype;
  long int *indexvals;
  long int *indexptrs;
  /* CSC indices */
  long int **rowvals;
  long int **colptrs;
  /* CSR indices */
  long int **colvals;
  long int **rowptrs;
};

typedef struct _SUNMatrixContent_Sparse *SUNMatrixContent_Sparse;


/*
 * -----------------------------------------------------------------
 * PART II: macros SM_CONTENT_S, SM_ROWS_S, SM_COLUMNS_S, SM_NNZ_S, 
 *          SM_NP_S, SM_SPARSETYPE_S, SM_DATA_S, SM_INDEXVALS_S, and
 *          SM_INDEXPTRS_S
 * -----------------------------------------------------------------
 * In the descriptions below, the following user declarations
 * are assumed:
 *
 * SUNMatrix A;
 * SUNMatrixContent_Sparse A_cont;
 * realtype *A_data;
 * long int A_nnz, A_np, A_type;
 * long int *A_ivals, *A_iptrs;
 *
 * (1) SM_CONTENT_S
 *
 *     This macro gives access to the contents of the sparse
 *     SUNMatrix
 *
 *     The assignment A_cont = SM_CONTENT_S(A) sets A_cont to be
 *     a pointer to the sparse SUNMatrix content structure.
 *
 * (2) SM_ROWS_D, SM_COLUMNS_D, SM_NNZ_S, SM_NP_S, SM_SPARSETYPE_S, 
 *     SM_DATA_S, SM_INDEXVALS_S and SM_INDEXPTRS_S
 *
 *     These macros give access to the individual parts of
 *     the content structure of a sparse SUNMatrix.
 *
 *     The assignment A_rows = SM_ROWS_S(A) sets A_rows to be
 *     the number of rows in A.
 *
 *     The assignment A_cols = SM_COLUMNS_S(A) sets A_cols to be
 *     the number of columns in A.
 *
 *     The assignment A_nnz = SM_NNZ_S(A) sets A_nnz to be
 *     the number of nonzero entries in A.
 *
 *     The assignment A_np = SM_NP_S(A) sets A_np to be
 *     the number of index pointers in A.
 *
 *     The assignment A_type = SM_SPARSETYPE_S(A) sets A_type to be
 *     the type of sparse matrix that A is (CSC_MAT or CSR_MAT).
 *
 *     The assignment A_data = SM_DATA_S(A) sets A_data to be
 *     a pointer to the first component of the data array for A. 
 *
 *     The assignment A_ivals = SM_INDEXVALS_S(A) sets A_ivals to be
 *     a pointer to the array of index values of each nonzero entry in A. 
 *
 *     The assignment A_iptrs = SM_INDEXPTRS_S(A) sets A_iptrs to be
 *     a pointer to the array of starting indices for the first entry
 *     of each row/column in the data/indexvals arrays.
 *
 * -----------------------------------------------------------------
 */

#define SM_CONTENT_S(A)     ( (SUNMatrixContent_Sparse)(A->content) )

#define SM_ROWS_S(A)        ( SM_CONTENT_S(A)->M )

#define SM_COLUMNS_S(A)     ( SM_CONTENT_S(A)->N )

#define SM_NNZ_S(A)         ( SM_CONTENT_S(A)->NNZ )

#define SM_NP_S(A)          ( SM_CONTENT_S(A)->NP )

#define SM_SPARSETYPE_S(A)  ( SM_CONTENT_S(A)->sparsetype )

#define SM_DATA_S(A)        ( SM_CONTENT_S(A)->data )

#define SM_INDEXVALS_S(A)   ( SM_CONTENT_S(A)->indexvals )

#define SM_INDEXPTRS_S(A)   ( SM_CONTENT_S(A)->indexptrs )

/*
 * -----------------------------------------------------------------
 * PART III: functions exported by sunmatrix_sparse
 * 
 * CONSTRUCTORS:
 *    SUNMatrixNew_Sparse
 *    SUNMatrix_DenseToSparse
 * DESTRUCTORS:
 *    SUNMatrixDestroy_Sparse
 * OTHER:
 *    SUNMatrixPrint_Sparse
 *    SparseReallocMat
 *    SUNMatrixSparse_Rows
 *    SUNMatrixSparse_Columns 
 *    SUNMatrixSparse_NNZ
 *    SUNMatrixSparse_Data 
 *    SUNMatrixSparse_IndexValues
 *    SUNMatrixSparse_IndexPointers

 * -----------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------
 * Function: SUNMatrixNew_Sparse
 * -----------------------------------------------------------------
 * SUNMatrixNew_Sparse creates and allocates memory for an M-by-N 
 * sparse SUNMatrix of type sparsetype.
 * Requirements: M and N must be strictly positive; NNZ must be 
 * non-negative; sparsetype must be either CSC_MAT or CSR_MAT;
 * Returns NULL if any requirements are violated, or if the matrix 
 * storage request cannot be satisfied.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT SUNMatrix SUNMatrixNew_Sparse(long int M, long int N,
                                              long int NNZ, int sparsetype);

/*
 * -----------------------------------------------------------------
 * Function: SUNMatrix_DenseToSparse
 * -----------------------------------------------------------------
 * SUNMatrix_DenseToSparse creates a new sparse matrix from an 
 * existing dense or band matrix by copying all values with 
 * magnitude larger than droptol into the sparse matrix structure.  
 * Requirements: A must have type SUNMATRIX_DENSE or SUNMATRIX_BAND; 
 * droptol must be non-negative; sparsetype must be either 
 * CSC_MAT or CSR_MAT.
 * Returns NULL if any requirements are violated, or if the matrix
 * storage request cannot be satisfied. 
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT SUNMatrix SUNMatrix_DenseToSparse(SUNMatrix A,
                                                  realtype droptol,
                                                  int sparsetype);

/*
 * -----------------------------------------------------------------
 * Functions: SparseReallocMat
 * -----------------------------------------------------------------
 * This function reallocates internal arrays so that the resulting 
 * sparse matrix holds colptrs[N] nonzeros.  Returns 0 on success and 
 * 1 on failure (e.g. if A does not have sparse type)
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT int SparseReallocMat(SUNMatrix A);

/*
 * -----------------------------------------------------------------
 * Functions: SUNMatrixPrint_Sparse
 * -----------------------------------------------------------------
 * This function prints the sparse matrix information to a file 
 * pointer.  It is intended as a debugging tool with small values 
 * of NNZ.  The elements are printed using the %g/%lg/%Lg option. 
 * A blank line is printed before and after the matrix.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void SUNMatrixPrint_Sparse(SUNMatrix A, FILE* outfile);


/*
 * -----------------------------------------------------------------
 * Accessor Functions: 
 *
 * SUNMatrixSparse_Rows 
 *    Returns the number of rows in the sparse matrix
 *
 * SUNMatrixSparse_Columns
 *    Returns the number of columns in the sparse matrix
 *
 * SUNMatrixSparse_NNZ
 *    Returns the allocated number of nonzeros in the sparse matrix
 *
 * SUNMatrixSparse_Data
 *    Returns a pointer to the data array for the sparse matrix
 *
 * SUNMatrixSparse_IndexValues
 *    Returns a ptr to the index value array for the sparse matrix:
 *    for CSR this is the column index for each nonzero,
 *    for CSC this is the row index for each nonzero.
 *
 * SUNMatrixSparse_IndexPointers
 *    Returns a ptr to the index pointer array for the sparse matrix:
 *    for CSR this is the location of the first entry of each row,
 *    for CSC this is the location of the first entry of each column.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT long int SUNMatrixSparse_Rows(SUNMatrix A);
SUNDIALS_EXPORT long int SUNMatrixSparse_Columns(SUNMatrix A);
SUNDIALS_EXPORT long int SUNMatrixSparse_NNZ(SUNMatrix A);
SUNDIALS_EXPORT realtype* SUNMatrixSparse_Data(SUNMatrix A);
SUNDIALS_EXPORT long int* SUNMatrixSparse_IndexValues(SUNMatrix A);
SUNDIALS_EXPORT long int* SUNMatrixSparse_IndexPointers(SUNMatrix A);

/*
 * -----------------------------------------------------------------
 * sparse implementations of various useful matrix operations
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT SUNMatrix_ID SUNMatrixGetID_Sparse(SUNMatrix A);
SUNDIALS_EXPORT SUNMatrix SUNMatrixClone_Sparse(SUNMatrix A);
SUNDIALS_EXPORT void SUNMatrixDestroy_Sparse(SUNMatrix A);
SUNDIALS_EXPORT int SUNMatrixZero_Sparse(SUNMatrix A);
SUNDIALS_EXPORT int SUNMatrixScale_Sparse(realtype c, SUNMatrix A);
SUNDIALS_EXPORT int SUNMatrixCopy_Sparse(SUNMatrix A, SUNMatrix B);
SUNDIALS_EXPORT int SUNMatrixAddIdentity_Sparse(SUNMatrix A);
SUNDIALS_EXPORT int SUNMatrixAdd_Sparse(SUNMatrix A, SUNMatrix B);
SUNDIALS_EXPORT int SUNMatrixMatvec_Sparse(SUNMatrix A, N_Vector x, N_Vector y);


#ifdef __cplusplus
}
#endif

#endif
