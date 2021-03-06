/*  File produced by Kranc */

#include "cctk.h"
#include "cctk_Arguments.h"
#include "cctk_Parameters.h"
#include "cctk_Faces.h"
#include "util_Table.h"
#include "Symmetry.h"


/* the boundary treatment is split into 3 steps:    */
/* 1. excision                                      */
/* 2. symmetries                                    */
/* 3. "other" boundary conditions, e.g. radiative */

/* to simplify scheduling and testing, the 3 steps  */
/* are currently applied in separate functions      */


extern "C" void SimpleWaveScript_CheckBoundaries(CCTK_ARGUMENTS)
{
  DECLARE_CCTK_ARGUMENTS;
  DECLARE_CCTK_PARAMETERS;
  
  return;
}

extern "C" void SimpleWaveScript_SelectBoundConds(CCTK_ARGUMENTS)
{
  DECLARE_CCTK_ARGUMENTS;
  DECLARE_CCTK_PARAMETERS;
  
  CCTK_INT ierr CCTK_ATTRIBUTE_UNUSED = 0;
  
  if (CCTK_EQUALS(phi_group_bound, "none"  ) ||
      CCTK_EQUALS(phi_group_bound, "static") ||
      CCTK_EQUALS(phi_group_bound, "flat"  ) ||
      CCTK_EQUALS(phi_group_bound, "zero"  ) )
  {
    ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, 1, -1,
                      "SimpleWaveScript::phi_group", phi_group_bound);
    if (ierr < 0)
       CCTK_WARN(0, "Failed to register phi_group_bound BC for SimpleWaveScript::phi_group!");
  }
  
  if (CCTK_EQUALS(pi_group_bound, "none"  ) ||
      CCTK_EQUALS(pi_group_bound, "static") ||
      CCTK_EQUALS(pi_group_bound, "flat"  ) ||
      CCTK_EQUALS(pi_group_bound, "zero"  ) )
  {
    ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, 1, -1,
                      "SimpleWaveScript::pi_group", pi_group_bound);
    if (ierr < 0)
       CCTK_WARN(0, "Failed to register pi_group_bound BC for SimpleWaveScript::pi_group!");
  }
  
  if (CCTK_EQUALS(phi_bound, "none"  ) ||
      CCTK_EQUALS(phi_bound, "static") ||
      CCTK_EQUALS(phi_bound, "flat"  ) ||
      CCTK_EQUALS(phi_bound, "zero"  ) )
  {
    ierr = Boundary_SelectVarForBC(cctkGH, CCTK_ALL_FACES, 1, -1,
                      "SimpleWaveScript::phi", phi_bound);
    if (ierr < 0)
       CCTK_WARN(0, "Failed to register phi_bound BC for SimpleWaveScript::phi!");
  }
  
  if (CCTK_EQUALS(pi_bound, "none"  ) ||
      CCTK_EQUALS(pi_bound, "static") ||
      CCTK_EQUALS(pi_bound, "flat"  ) ||
      CCTK_EQUALS(pi_bound, "zero"  ) )
  {
    ierr = Boundary_SelectVarForBC(cctkGH, CCTK_ALL_FACES, 1, -1,
                      "SimpleWaveScript::pi", pi_bound);
    if (ierr < 0)
       CCTK_WARN(0, "Failed to register pi_bound BC for SimpleWaveScript::pi!");
  }
  
  if (CCTK_EQUALS(phi_group_bound, "radiative"))
  {
   /* select radiation boundary condition */
    static CCTK_INT handle_phi_group_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_phi_group_bound < 0) handle_phi_group_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_phi_group_bound < 0) CCTK_WARN(0, "could not create table!");
    if (Util_TableSetReal(handle_phi_group_bound , phi_group_bound_limit, "LIMIT") < 0)
       CCTK_WARN(0, "could not set LIMIT value in table!");
    if (Util_TableSetReal(handle_phi_group_bound ,phi_group_bound_speed, "SPEED") < 0)
       CCTK_WARN(0, "could not set SPEED value in table!");
  
    ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, 1, handle_phi_group_bound, 
                      "SimpleWaveScript::phi_group", "Radiation");
  
    if (ierr < 0)
       CCTK_WARN(0, "Failed to register Radiation BC for SimpleWaveScript::phi_group!");
  
  }
  
  if (CCTK_EQUALS(pi_group_bound, "radiative"))
  {
   /* select radiation boundary condition */
    static CCTK_INT handle_pi_group_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_pi_group_bound < 0) handle_pi_group_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_pi_group_bound < 0) CCTK_WARN(0, "could not create table!");
    if (Util_TableSetReal(handle_pi_group_bound , pi_group_bound_limit, "LIMIT") < 0)
       CCTK_WARN(0, "could not set LIMIT value in table!");
    if (Util_TableSetReal(handle_pi_group_bound ,pi_group_bound_speed, "SPEED") < 0)
       CCTK_WARN(0, "could not set SPEED value in table!");
  
    ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, 1, handle_pi_group_bound, 
                      "SimpleWaveScript::pi_group", "Radiation");
  
    if (ierr < 0)
       CCTK_WARN(0, "Failed to register Radiation BC for SimpleWaveScript::pi_group!");
  
  }
  
  if (CCTK_EQUALS(phi_bound, "radiative"))
  {
   /* select radiation boundary condition */
    static CCTK_INT handle_phi_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_phi_bound < 0) handle_phi_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_phi_bound < 0) CCTK_WARN(0, "could not create table!");
    if (Util_TableSetReal(handle_phi_bound , phi_bound_limit, "LIMIT") < 0)
       CCTK_WARN(0, "could not set LIMIT value in table!");
    if (Util_TableSetReal(handle_phi_bound ,phi_bound_speed, "SPEED") < 0)
        CCTK_WARN(0, "could not set SPEED value in table!");
  
    ierr = Boundary_SelectVarForBC(cctkGH, CCTK_ALL_FACES, 1, handle_phi_bound, 
                      "SimpleWaveScript::phi", "Radiation");
  
    if (ierr < 0)
       CCTK_WARN(0, "Failed to register Radiation BC for SimpleWaveScript::phi!");
  
  }
  
  if (CCTK_EQUALS(pi_bound, "radiative"))
  {
   /* select radiation boundary condition */
    static CCTK_INT handle_pi_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_pi_bound < 0) handle_pi_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_pi_bound < 0) CCTK_WARN(0, "could not create table!");
    if (Util_TableSetReal(handle_pi_bound , pi_bound_limit, "LIMIT") < 0)
       CCTK_WARN(0, "could not set LIMIT value in table!");
    if (Util_TableSetReal(handle_pi_bound ,pi_bound_speed, "SPEED") < 0)
        CCTK_WARN(0, "could not set SPEED value in table!");
  
    ierr = Boundary_SelectVarForBC(cctkGH, CCTK_ALL_FACES, 1, handle_pi_bound, 
                      "SimpleWaveScript::pi", "Radiation");
  
    if (ierr < 0)
       CCTK_WARN(0, "Failed to register Radiation BC for SimpleWaveScript::pi!");
  
  }
  
  if (CCTK_EQUALS(phi_group_bound, "scalar"))
  {
   /* select scalar boundary condition */
    static CCTK_INT handle_phi_group_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_phi_group_bound < 0) handle_phi_group_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_phi_group_bound < 0) CCTK_WARN(0, "could not create table!");
    if (Util_TableSetReal(handle_phi_group_bound ,phi_group_bound_scalar, "SCALAR") < 0)
        CCTK_WARN(0, "could not set SCALAR value in table!");
  
    ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, 1, handle_phi_group_bound, 
                      "SimpleWaveScript::phi_group", "scalar");
  
    if (ierr < 0)
       CCTK_WARN(0, "Failed to register Scalar BC for SimpleWaveScript::phi_group!");
  
  }
  
  if (CCTK_EQUALS(pi_group_bound, "scalar"))
  {
   /* select scalar boundary condition */
    static CCTK_INT handle_pi_group_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_pi_group_bound < 0) handle_pi_group_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_pi_group_bound < 0) CCTK_WARN(0, "could not create table!");
    if (Util_TableSetReal(handle_pi_group_bound ,pi_group_bound_scalar, "SCALAR") < 0)
        CCTK_WARN(0, "could not set SCALAR value in table!");
  
    ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, 1, handle_pi_group_bound, 
                      "SimpleWaveScript::pi_group", "scalar");
  
    if (ierr < 0)
       CCTK_WARN(0, "Failed to register Scalar BC for SimpleWaveScript::pi_group!");
  
  }
  
  if (CCTK_EQUALS(phi_bound, "scalar"))
  {
   /* select scalar boundary condition */
    static CCTK_INT handle_phi_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_phi_bound < 0) handle_phi_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_phi_bound < 0) CCTK_WARN(0, "could not create table!");
    if (Util_TableSetReal(handle_phi_bound ,phi_bound_scalar, "SCALAR") < 0)
      CCTK_WARN(0, "could not set SCALAR value in table!");
  
    ierr = Boundary_SelectVarForBC(cctkGH, CCTK_ALL_FACES, 1, handle_phi_bound, 
                      "SimpleWaveScript::phi", "scalar");
  
    if (ierr < 0)
       CCTK_WARN(0, "Error in registering Scalar BC for SimpleWaveScript::phi!");
  
  }
  
  if (CCTK_EQUALS(pi_bound, "scalar"))
  {
   /* select scalar boundary condition */
    static CCTK_INT handle_pi_bound CCTK_ATTRIBUTE_UNUSED = -1;
    if (handle_pi_bound < 0) handle_pi_bound = Util_TableCreate(UTIL_TABLE_FLAGS_CASE_INSENSITIVE);
    if (handle_pi_bound < 0) CCTK_WARN(0, "could not create table!");
    if (Util_TableSetReal(handle_pi_bound ,pi_bound_scalar, "SCALAR") < 0)
      CCTK_WARN(0, "could not set SCALAR value in table!");
  
    ierr = Boundary_SelectVarForBC(cctkGH, CCTK_ALL_FACES, 1, handle_pi_bound, 
                      "SimpleWaveScript::pi", "scalar");
  
    if (ierr < 0)
       CCTK_WARN(0, "Error in registering Scalar BC for SimpleWaveScript::pi!");
  
  }
  return;
}



/* template for entries in parameter file:
#$bound$#SimpleWaveScript::phi_group_bound       = "skip"
#$bound$#SimpleWaveScript::phi_group_bound_speed = 1.0
#$bound$#SimpleWaveScript::phi_group_bound_limit = 0.0
#$bound$#SimpleWaveScript::phi_group_bound_scalar = 0.0

#$bound$#SimpleWaveScript::pi_group_bound       = "skip"
#$bound$#SimpleWaveScript::pi_group_bound_speed = 1.0
#$bound$#SimpleWaveScript::pi_group_bound_limit = 0.0
#$bound$#SimpleWaveScript::pi_group_bound_scalar = 0.0

#$bound$#SimpleWaveScript::phi_bound       = "skip"
#$bound$#SimpleWaveScript::phi_bound_speed = 1.0
#$bound$#SimpleWaveScript::phi_bound_limit = 0.0
#$bound$#SimpleWaveScript::phi_bound_scalar = 0.0

#$bound$#SimpleWaveScript::pi_bound       = "skip"
#$bound$#SimpleWaveScript::pi_bound_speed = 1.0
#$bound$#SimpleWaveScript::pi_bound_limit = 0.0
#$bound$#SimpleWaveScript::pi_bound_scalar = 0.0

*/

