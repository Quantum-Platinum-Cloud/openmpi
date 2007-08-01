/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ompi_config.h"

#include "ompi/mpi/c/bindings.h"

#if OMPI_HAVE_WEAK_SYMBOLS && OMPI_PROFILING_DEFINES
#pragma weak MPI_Errhandler_free = PMPI_Errhandler_free
#endif

#if OMPI_PROFILING_DEFINES
#include "ompi/mpi/c/profile/defines.h"
#endif

static const char FUNC_NAME[] = "MPI_Errhandler_free";


int MPI_Errhandler_free(MPI_Errhandler *errhandler)
{
  /* Error checking */

  if (MPI_PARAM_CHECK) {
    OMPI_ERR_INIT_FINALIZE(FUNC_NAME);
    /* Raise an MPI exception if we got NULL or if we got an intrinsic
       *and* the reference count is 1 (meaning that this FREE would
       actually free the underlying intrinsic object).  This is ugly
       but necessary -- see below. */
    if (NULL == errhandler ||
        (ompi_errhandler_is_intrinsic(*errhandler) && 
         1 == (*errhandler)->super.obj_reference_count)) {
      return OMPI_ERRHANDLER_INVOKE(MPI_COMM_WORLD, MPI_ERR_ARG,
                                   "MPI_Errhandler_free");
    }
  }

  /* Return the errhandler.  According to MPI-2 errata, any errhandler
     obtained by MPI_*_GET_ERRHANDLER or MPI_ERRHANDLER_GET must also
     be freed by MPI_ERRHANDLER_FREE (including intrinsic error
     handlers).  For example, this is valid:

     int main() {
         MPI_Errhandler errhdl;
         MPI_Init(NULL, NULL);
         MPI_Comm_get_errhandler(MPI_COMM_WORLD, &errhdl);
         MPI_Errhandler_free(&errhdl);
         MPI_Finalize();
         return 0;
     }
     
     So decrease the refcount here. */

  OBJ_RELEASE(*errhandler);
  *errhandler = MPI_ERRHANDLER_NULL;

  /* All done */

  return MPI_SUCCESS;
}
